#include "Chicane/Core/Layer/LevelLayer.hpp"

#include "Chicane/Box.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Game/Transformable/Component/MeshComponent.hpp"

namespace Chicane
{
    LevelLayer::LevelLayer(Window::Instance* inWindow)
        : Layer("Level"),
        m_clearValues({})
    {
        m_rendererInternals = inWindow->getRendererInternals();

        m_clearValues.push_back(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f));
        m_clearValues.push_back(vk::ClearDepthStencilValue(1.0f, 0));

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
        m_rendererInternals.logicalDevice.waitIdle();

        destroyTextureResources();
        destroyModelData();

        m_graphicsPipeline.reset();
    }

    bool LevelLayer::canRender() const
    {
        return m_textureManager->canBind() && m_modelManager->canDraw() && !m_meshes.empty();
    }

    void LevelLayer::build()
    {
        if (m_bIsInitialized || m_textureManager->isEmpty() || m_modelManager->isEmpty() || m_meshes.empty())
        {
            return;
        }

        initFrameResources();
        initTextureResources();
        initGraphicsPipeline();
        initFramebuffers();

        m_bIsInitialized = true;

        buildTextures();
        buildModelData();
        setupFrames();
    }

    void LevelLayer::destroy()
    {
        if (!m_bIsInitialized)
        {
            return;
        }

        destroyFrameResources();
    }

    void LevelLayer::rebuild()
    {
        if (!m_bIsInitialized)
        {
            return;
        }

        initFrameResources();
        initFramebuffers();
        setupFrames();
    }

    void LevelLayer::setup(Frame::Instance& outFrame)
    {
        if (!canRender())
        {
            return;
        }

        outFrame.updateMeshData(m_meshes);
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

        vk::RenderPassBeginInfo renderPassBeginInfo {};
        renderPassBeginInfo.renderPass          = m_graphicsPipeline->renderPass;
        renderPassBeginInfo.framebuffer         = outFrame.getFramebuffer(m_id);
        renderPassBeginInfo.renderArea.offset.x = 0;
        renderPassBeginInfo.renderArea.offset.y = 0;
        renderPassBeginInfo.renderArea.extent   = inSwapChainExtent;
        renderPassBeginInfo.clearValueCount     = static_cast<uint32_t>(m_clearValues.size());
        renderPassBeginInfo.pClearValues        = m_clearValues.data();

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
                m_graphicsPipeline->layout,
                m_textureDescriptor
            );

            m_modelManager->drawAll(inCommandBuffer);
        inCommandBuffer.endRenderPass();
    }

    void LevelLayer::loadEvents()
    {
        if (m_bIsInitialized)
        {
            return;
        }

        m_level->watchComponents(
            std::bind(
                &LevelLayer::updateMeshes,
                this,
                std::placeholders::_1
            )
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
                if (inSubject == Model::Manager::EventSubject::Allocation)
                {
                    rebuildModelData();
                }
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
            m_rendererInternals.logicalDevice,
            frameLayoutBidings
        );

        Descriptor::PoolCreateInfo descriptorPoolCreateInfo;
        descriptorPoolCreateInfo.size  = static_cast<uint32_t>(m_rendererInternals.swapchain->frames.size());
        descriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eUniformBuffer);
        descriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eStorageBuffer);

        Descriptor::initPool(
            m_frameDescriptor.pool,
            m_rendererInternals.logicalDevice,
            descriptorPoolCreateInfo
        );

        for (Frame::Instance& frame : m_rendererInternals.swapchain->frames)
        {
            vk::DescriptorSet levelDescriptorSet;
            Descriptor::allocalteSet(
                levelDescriptorSet,
                m_rendererInternals.logicalDevice,
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
        if (!m_bIsInitialized)
        {
            return;
        }

        m_rendererInternals.logicalDevice.destroyDescriptorSetLayout(
            m_frameDescriptor.setLayout
        );
        m_rendererInternals.logicalDevice.destroyDescriptorPool(
            m_frameDescriptor.pool
        );
    }

    void LevelLayer::initTextureResources()
    {
        if (m_bIsInitialized)
        {
            return;
        }

        Descriptor::SetLayoutBidingsCreateInfo materialLayoutBidings;
        materialLayoutBidings.count = 1;

        materialLayoutBidings.indices.push_back(0);
        materialLayoutBidings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
        materialLayoutBidings.counts.push_back(m_textureManager->getCount());
        materialLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eFragment);

        Descriptor::initSetLayout(
            m_textureDescriptor.setLayout,
            m_rendererInternals.logicalDevice,
            materialLayoutBidings
        );

        Descriptor::PoolCreateInfo descriptorPoolCreateInfo;
        descriptorPoolCreateInfo.size = TEXTURE_MAX_COUNT;
        descriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eCombinedImageSampler);

        Descriptor::initPool(
            m_textureDescriptor.pool,
            m_rendererInternals.logicalDevice,
            descriptorPoolCreateInfo
        );
    }

    void LevelLayer::destroyTextureResources()
    {
        if (!m_bIsInitialized)
        {
            return;
        }

        m_rendererInternals.logicalDevice.destroyDescriptorSetLayout(
            m_textureDescriptor.setLayout
        );

        m_rendererInternals.logicalDevice.destroyDescriptorPool(
            m_textureDescriptor.pool
        );
    }

    void LevelLayer::initGraphicsPipeline()
    {
        if (m_bIsInitialized)
        {
            return;
        }

        GraphicsPipeline::Attachment colorAttachment {};
        colorAttachment.format        = m_rendererInternals.swapchain->format;
        colorAttachment.loadOp        = vk::AttachmentLoadOp::eLoad;
        colorAttachment.initialLayout = vk::ImageLayout::ePresentSrcKHR;

        GraphicsPipeline::Attachment depthAttachment {};
        depthAttachment.format        = m_rendererInternals.swapchain->depthFormat;
        depthAttachment.loadOp        = vk::AttachmentLoadOp::eClear;
        depthAttachment.initialLayout = vk::ImageLayout::eUndefined;

        GraphicsPipeline::CreateInfo createInfo {};
        createInfo.bHasVertices          = true;
        createInfo.bHasDepth             = true;
        createInfo.logicalDevice         = m_rendererInternals.logicalDevice;
        createInfo.vertexShaderPath      = "Content/Engine/Shaders/level.vert.spv";
        createInfo.fragmentShaderPath    = "Content/Engine/Shaders/level.frag.spv";
        createInfo.bindingDescription    = Vertex::getBindingDescription();
        createInfo.attributeDescriptions = Vertex::getAttributeDescriptions();
        createInfo.extent                = m_rendererInternals.swapchain->extent;
        createInfo.descriptorSetLayouts  = { m_frameDescriptor.setLayout, m_textureDescriptor.setLayout };
        createInfo.colorAttachment       = colorAttachment;
        createInfo.depthAttachment       = depthAttachment;
        createInfo.polygonMode           = vk::PolygonMode::eFill;

        m_graphicsPipeline = std::make_unique<GraphicsPipeline::Instance>(createInfo);
    }

    void LevelLayer::initFramebuffers()
    {
        for (Frame::Instance& frame : m_rendererInternals.swapchain->frames)
        {
            Frame::Buffer::CreateInfo framebufferCreateInfo {};
            framebufferCreateInfo.id              = m_id;
            framebufferCreateInfo.logicalDevice   = m_rendererInternals.logicalDevice;
            framebufferCreateInfo.renderPass      = m_graphicsPipeline->renderPass;
            framebufferCreateInfo.swapChainExtent = m_rendererInternals.swapchain->extent;
            framebufferCreateInfo.attachments.push_back(frame.imageView);
            framebufferCreateInfo.attachments.push_back(frame.depth.imageView);

            Frame::Buffer::init(frame, framebufferCreateInfo);
        }
    }

    void LevelLayer::buildTextures()
    {
        if (!m_bIsInitialized)
        {
            return;
        }

        m_textureManager->build(
            m_rendererInternals.logicalDevice,
            m_rendererInternals.physicalDevice,
            m_rendererInternals.mainCommandBuffer,
            m_rendererInternals.graphicsQueue,
            m_textureDescriptor
        );
    }

    void LevelLayer::buildModelData()
    {
        if (!m_bIsInitialized)
        {
            return;
        }

        m_modelManager->build(
            m_modelVertexBuffer,
            m_modelIndexBuffer,
            m_rendererInternals.logicalDevice,
            m_rendererInternals.physicalDevice,
            m_rendererInternals.graphicsQueue,
            m_rendererInternals.mainCommandBuffer
        );
    }

    void LevelLayer::destroyModelData()
    {
        if (!m_bIsInitialized)
        {
            return;
        }

        m_rendererInternals.logicalDevice.waitIdle();

        Buffer::destroy(
            m_rendererInternals.logicalDevice,
            m_modelVertexBuffer
        );
        Buffer::destroy(
            m_rendererInternals.logicalDevice,
            m_modelIndexBuffer
        );
    }

    void LevelLayer::rebuildModelData()
    {
        if (!m_bIsInitialized)
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
        if (!m_bIsInitialized)
        {
            return;
        }

        for (Frame::Instance& frame : m_rendererInternals.swapchain->frames)
        {
            frame.setupMeshData(m_meshes);
        }
    }

    void LevelLayer::setFramesAsDirty()
    {
        if (!m_bIsInitialized)
        {
            return;
        }

        for (Frame::Instance& frame : m_rendererInternals.swapchain->frames)
        {
            frame.setAsDirty();
        }
    }

    void LevelLayer::updateMeshes(const std::vector<Component*>& inComponents)
    {
        std::vector<MeshComponent*> nextMeshes {};
        nextMeshes.reserve(inComponents.size());

        for (Component* component : inComponents)
        {
            if (typeid(*component) != typeid(MeshComponent))
            {
                continue;
            }

            MeshComponent* mesh = static_cast<MeshComponent*>(component);

            if (!mesh->isDrawable())
            {
                continue;
            }

            nextMeshes.emplace_back(mesh);
        }

        if (m_meshes.size() == nextMeshes.size())
        {
            return;
        }

        m_meshes = nextMeshes;

        if (!m_bIsInitialized)
        {
            build();

            return;
        }

        setFramesAsDirty();
    }
}