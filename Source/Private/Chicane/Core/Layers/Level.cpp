#include "Chicane/Core/Layers/Level.hpp"

#include "Chicane/Box.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Game/Actor/Component/Mesh.hpp"

namespace Chicane
{
    LevelLayer::LevelLayer(Window* inWindow)
        : Layer("Level")
    {
        m_internals = inWindow->getRendererInternals();

        if (!hasActiveLevel())
        {
            return;
        }

        m_level = getActiveLevel();

        m_textureManager = Loader::getTextureManager();
        m_modelManager   = Loader::getModelManager();

        loadEvents();
    }

    LevelLayer::~LevelLayer()
    {
        m_internals.logicalDevice.waitIdle();

        destroyTextureResources();
        destroyModelData();

        m_graphicsPipeline.reset();
    }

    bool LevelLayer::canRender() const
    {
        return m_textureManager->canBind() && m_modelManager->canDraw();
    }

    void LevelLayer::build()
    {
        if (m_textureManager->isEmpty() || m_modelManager->isEmpty() || m_isInitialized)
        {
            return;
        }

        initFrameResources();
        initTextureResources();
        initGraphicsPipeline();
        initFramebuffers();

        m_isInitialized = true;

        buildTextures();
        buildModelData();

        updateMeshes();
    }

    void LevelLayer::destroy()
    {
        if (!m_isInitialized)
        {
            return;
        }

        destroyFrameResources();
    }

    void LevelLayer::rebuild()
    {
        if (!m_isInitialized)
        {
            return;
        }

        initFramebuffers();
        initFrameResources();

        setupFrames();
    }

    void LevelLayer::setup(Frame::Instance& outFrame)
    {
        if (!canRender())
        {
            return;
        }

        outFrame.updateModelData(m_meshes);
    }

    void LevelLayer::render(
        Frame::Instance& outFrame,
        const vk::CommandBuffer& inCommandBuffer,
        const vk::Extent2D& inSwapChainExtent
    )
    {
        if (!canRender())
        {
            return;
        }

        std::vector<vk::ClearValue> clearValues;
        clearValues.push_back(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f));
        clearValues.push_back(vk::ClearDepthStencilValue(1.0f, 0));

        vk::RenderPassBeginInfo renderPassBeginInfo {};
        renderPassBeginInfo.renderPass          = m_graphicsPipeline->renderPass;
        renderPassBeginInfo.framebuffer         = outFrame.getFramebuffer(m_id);
        renderPassBeginInfo.renderArea.offset.x = 0;
        renderPassBeginInfo.renderArea.offset.y = 0;
        renderPassBeginInfo.renderArea.extent   = inSwapChainExtent;
        renderPassBeginInfo.clearValueCount     = static_cast<uint32_t>(clearValues.size());
        renderPassBeginInfo.pClearValues        = clearValues.data();

        inCommandBuffer.beginRenderPass(
            &renderPassBeginInfo,
            vk::SubpassContents::eInline
        );
            inCommandBuffer.bindPipeline(
                vk::PipelineBindPoint::eGraphics,
                m_graphicsPipeline->instance
            );

            inCommandBuffer.bindDescriptorSets(
                vk::PipelineBindPoint::eGraphics,
                m_graphicsPipeline->layout,
                0,
                outFrame.getDescriptorSet(m_id),
                nullptr
            );

            vk::Buffer vertexBuffers[] = { m_modelVertexBuffer.instance };
            vk::DeviceSize offsets[]   = { 0 };

            inCommandBuffer.bindVertexBuffers(
                0,
                1,
                vertexBuffers,
                offsets
            );

            inCommandBuffer.bindIndexBuffer(
                m_modelIndexBuffer.instance,
                0,
                vk::IndexType::eUint32
            );

            m_textureManager->bindAll(
                inCommandBuffer,
                m_graphicsPipeline->layout
            );

            m_modelManager->drawAll(inCommandBuffer);
        inCommandBuffer.endRenderPass();
    }

    void LevelLayer::loadEvents()
    {
        if (m_isInitialized)
        {
            return;
        }

        m_level->watchComponents(
            [&](ActorComponent* inComponent)
            {
                updateMeshes();
            }
        );

        m_textureManager->watchChanges(
            [&](void* inParam)
            {
                if (m_textureManager->isEmpty())
                {
                    return;
                }

                buildTextures();
            }
        );

        m_modelManager->watchChanges(
            [&](Model::Manager::EventSubject inSubject)
            {
                if (!m_isInitialized)
                {
                    build();

                    return;
                }

                if (inSubject == Model::Manager::EventSubject::Allocation)
                {
                    rebuildModelData();
                }

                updateMeshes();
            }
        );
    }

    void LevelLayer::initFrameResources()
    {
        Descriptor::SetLayoutBidingsCreateInfo frameLayoutBidings;
        frameLayoutBidings.count = 2;

        /// Camera
        frameLayoutBidings.indices.push_back(0);
        frameLayoutBidings.types.push_back(vk::DescriptorType::eUniformBuffer);
        frameLayoutBidings.counts.push_back(1);
        frameLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

        /// Model
        frameLayoutBidings.indices.push_back(1);
        frameLayoutBidings.types.push_back(vk::DescriptorType::eStorageBuffer);
        frameLayoutBidings.counts.push_back(1);
        frameLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

        Descriptor::initSetLayout(
            m_frameDescriptor.setLayout,
            m_internals.logicalDevice,
            frameLayoutBidings
        );

        Descriptor::PoolCreateInfo descriptorPoolCreateInfo;
        descriptorPoolCreateInfo.size  = static_cast<uint32_t>(m_internals.swapchain->frames.size());
        descriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eUniformBuffer);
        descriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eStorageBuffer);

        Descriptor::initPool(
            m_frameDescriptor.pool,
            m_internals.logicalDevice,
            descriptorPoolCreateInfo
        );

        for (Frame::Instance& frame : m_internals.swapchain->frames)
        {
            vk::DescriptorSet levelDescriptorSet;
            Descriptor::allocalteSetLayout(
                levelDescriptorSet,
                m_internals.logicalDevice,
                m_frameDescriptor.setLayout,
                m_frameDescriptor.pool
            );
            frame.addDescriptorSet(m_id, levelDescriptorSet);
    
            vk::WriteDescriptorSet cameraWriteInfo;
            cameraWriteInfo.dstSet          = levelDescriptorSet;
            cameraWriteInfo.dstBinding      = 0;
            cameraWriteInfo.dstArrayElement = 0;
            cameraWriteInfo.descriptorCount = 1;
            cameraWriteInfo.descriptorType  = vk::DescriptorType::eUniformBuffer;
            cameraWriteInfo.pBufferInfo     = &frame.cameraDescriptorBufferInfo;
            frame.addWriteDescriptorSet(cameraWriteInfo);

            vk::WriteDescriptorSet modelWriteInfo;
            modelWriteInfo.dstSet          = levelDescriptorSet;
            modelWriteInfo.dstBinding      = 1;
            modelWriteInfo.dstArrayElement = 0;
            modelWriteInfo.descriptorCount = 1;
            modelWriteInfo.descriptorType  = vk::DescriptorType::eStorageBuffer;
            modelWriteInfo.pBufferInfo     = &frame.modelDescriptorBufferInfo;
            frame.addWriteDescriptorSet(modelWriteInfo);
        }
    }

    void LevelLayer::destroyFrameResources()
    {
        if (!m_isInitialized)
        {
            return;
        }

        m_internals.logicalDevice.destroyDescriptorSetLayout(
            m_frameDescriptor.setLayout
        );
        m_internals.logicalDevice.destroyDescriptorPool(
            m_frameDescriptor.pool
        );
    }

    void LevelLayer::initTextureResources()
    {
        if (m_isInitialized)
        {
            return;
        }

        Descriptor::SetLayoutBidingsCreateInfo materialLayoutBidings;
        materialLayoutBidings.count = 1;

        materialLayoutBidings.indices.push_back(0);
        materialLayoutBidings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
        materialLayoutBidings.counts.push_back(1);
        materialLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eFragment);

        Descriptor::initSetLayout(
            m_texturelDescriptor.setLayout,
            m_internals.logicalDevice,
            materialLayoutBidings
        );

        Descriptor::PoolCreateInfo descriptorPoolCreateInfo;
        descriptorPoolCreateInfo.size = MAX_TEXTURE_COUNT;
        descriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eCombinedImageSampler);

        Descriptor::initPool(
            m_texturelDescriptor.pool,
            m_internals.logicalDevice,
            descriptorPoolCreateInfo
        );
    }

    void LevelLayer::destroyTextureResources()
    {
        if (!m_isInitialized)
        {
            return;
        }

        m_internals.logicalDevice.destroyDescriptorSetLayout(
            m_texturelDescriptor.setLayout
        );

        m_internals.logicalDevice.destroyDescriptorPool(
            m_texturelDescriptor.pool
        );
    }

    void LevelLayer::initGraphicsPipeline()
    {
        if (m_isInitialized)
        {
            return;
        }

        GraphicsPipeline::CreateInfo createInfo {};
        createInfo.canOverwrite          = true;
        createInfo.hasVertices           = true;
        createInfo.hasDepth              = true;
        createInfo.logicalDevice         = m_internals.logicalDevice;
        createInfo.vertexShaderPath      = "Content/Engine/Shaders/level.vert.spv";
        createInfo.fragmentShaderPath    = "Content/Engine/Shaders/level.frag.spv";
        createInfo.bindingDescription    = Vertex::getBindingDescription();
        createInfo.attributeDescriptions = Vertex::getAttributeDescriptions();
        createInfo.swapChainExtent       = m_internals.swapchain->extent;
        createInfo.swapChainImageFormat  = m_internals.swapchain->format;
        createInfo.depthFormat           = m_internals.swapchain->depthFormat;
        createInfo.descriptorSetLayouts  = { m_frameDescriptor.setLayout, m_texturelDescriptor.setLayout };
        createInfo.polygonMode           = vk::PolygonMode::eFill;

        m_graphicsPipeline = std::make_unique<GraphicsPipeline::Instance>(createInfo);
    }

    void LevelLayer::initFramebuffers()
    {
        for (Frame::Instance& frame : m_internals.swapchain->frames)
        {
            Frame::Buffer::CreateInfo framebufferCreateInfo {};
            framebufferCreateInfo.id              = m_id;
            framebufferCreateInfo.logicalDevice   = m_internals.logicalDevice;
            framebufferCreateInfo.renderPass      = m_graphicsPipeline->renderPass;
            framebufferCreateInfo.swapChainExtent = m_internals.swapchain->extent;
            framebufferCreateInfo.attachments.push_back(frame.imageView);
            framebufferCreateInfo.attachments.push_back(frame.depth.imageView);

            Frame::Buffer::init(frame, framebufferCreateInfo);
        }
    }

    void LevelLayer::buildTextures()
    {
        if (!m_isInitialized)
        {
            return;
        }

        m_textureManager->build(
            m_internals.logicalDevice,
            m_internals.physicalDevice,
            m_internals.mainCommandBuffer,
            m_internals.graphicsQueue,
            m_texturelDescriptor.setLayout,
            m_texturelDescriptor.pool
        );
    }

    void LevelLayer::buildModelData()
    {
        if (!m_isInitialized)
        {
            return;
        }

        m_modelManager->build(
            m_modelVertexBuffer,
            m_modelIndexBuffer,
            m_internals.logicalDevice,
            m_internals.physicalDevice,
            m_internals.graphicsQueue,
            m_internals.mainCommandBuffer
        );
    }

    void LevelLayer::destroyModelData()
    {
        if (!m_isInitialized)
        {
            return;
        }

        m_internals.logicalDevice.waitIdle();

        Buffer::destroy(
            m_internals.logicalDevice,
            m_modelVertexBuffer
        );
        Buffer::destroy(
            m_internals.logicalDevice,
            m_modelIndexBuffer
        );
    }

    void LevelLayer::rebuildModelData()
    {
        if (!m_isInitialized)
        {
            return;
        }

        destroyModelData();

        if (m_modelManager->isEmpty())
        {
            return;
        }

        buildModelData();
    }

    void LevelLayer::setupFrames()
    {
        if (!m_isInitialized)
        {
            return;
        }

        for (Frame::Instance& frame : m_internals.swapchain->frames)
        {
            frame.setupModelData(m_meshes);
        }
    }

    void LevelLayer::setFramesAsDirty()
    {
        if (!m_isInitialized)
        {
            return;
        }

        for (Frame::Instance& frame : m_internals.swapchain->frames)
        {
            frame.setAsDirty();
        }
    }

    void LevelLayer::updateMeshes()
    {
        if (!m_isInitialized)
        {
            return;
        }

        std::vector<MeshComponent*> latestMeshes = m_level->getComponents<MeshComponent>();

        std::uint32_t currentMeshCount = m_meshes.size();
        m_meshes.clear();

        m_meshes.reserve(latestMeshes.size());

        for (MeshComponent* mesh : latestMeshes)
        {
            if (!mesh->isDrawable())
            {
                continue;
            }

            m_meshes.push_back(mesh);
        }

        if (m_meshes.size() != currentMeshCount)
        {
            setFramesAsDirty();
        }
    }
}