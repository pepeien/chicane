#include "Chicane/Core/Layers/Level.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Game/Actor/Component.hpp"

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

        m_isInitialized = m_level->hasMeshes();

        m_textureManager = Allocator::getTextureManager();
        m_modelManager   = Allocator::getModelManager();

        loadEvents();
    }

    LevelLayer::~LevelLayer()
    {
        m_internals.logicalDevice.waitIdle();

        // Graphics Pipeline
        m_graphicsPipeline.reset();

        deleteFrameDescriptorSetLayout();
        destroyMaterialDescriptorSetLayout();
        destroyTextures();
        destroyMeshes();
    }

    void LevelLayer::build()
    {
        if (!m_isInitialized)
        {
            return;
        }

        initFrameDescriptorSetLayout();
        initMaterialDescriptorSetLayout();
        initGraphicsPipeline();
        initFramebuffers();
        initFrameResources();
        initTextures();
        buildTextures();
        initMeshes();
        buildMeshes();
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
        initMeshes();
    }

    void LevelLayer::setup(Frame::Instance& outFrame)
    {
        if (!m_isInitialized)
        {
            return;
        }

        outFrame.updateModelData(m_level);

        memcpy(
            outFrame.modelData.writeLocation,
            outFrame.modelData.transforms.data(),
            outFrame.modelData.allocationSize
        );
    }

    void LevelLayer::render(
        Frame::Instance& outFrame,
        const vk::CommandBuffer& inCommandBuffer,
        const vk::Extent2D& inSwapChainExtent
    )
    {
        if (!m_isInitialized)
        {
            return;
        }

        // Renderpass
        std::vector<vk::ClearValue> clearValues;
        clearValues.push_back(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 0.0f));
        clearValues.push_back(vk::ClearDepthStencilValue(1.f, 0u));

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

        // Preparing
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

        Allocator::getTextureManager()->bindAll(
            inCommandBuffer,
            m_graphicsPipeline->layout
        );

        Allocator::getModelManager()->drawAll(inCommandBuffer);

        inCommandBuffer.endRenderPass();

        tickAll();
    }

    void LevelLayer::loadEvents()
    {
        m_level->watchMeshes(
            [this](MeshComponent* inMesh)
            {
                if (!m_level->hasMeshes())
                {
                    m_isInitialized = false;

                    return;
                }

                if (!m_isInitialized)
                {
                    m_isInitialized = true;

                    build();

                    return;
                }

                m_internals.logicalDevice.waitIdle();

                buildTextures();

                destroyMeshes();
                initMeshes();
                buildMeshes();
            }
        );
    }

    void LevelLayer::initFrameDescriptorSetLayout()
    {
        if (!m_isInitialized)
        {
            return;
        }

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
    }
    
    void LevelLayer::deleteFrameDescriptorSetLayout()
    {
        m_internals.logicalDevice.destroyDescriptorSetLayout(
            m_frameDescriptor.setLayout
        );
    }

    void LevelLayer::initMaterialDescriptorSetLayout()
    {
        if (!m_isInitialized)
        {
            return;
        }

        Descriptor::SetLayoutBidingsCreateInfo materialLayoutBidings;
        materialLayoutBidings.count = 1;

        /// Texture
        materialLayoutBidings.indices.push_back(0);
        materialLayoutBidings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
        materialLayoutBidings.counts.push_back(1);
        materialLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eFragment);

        Descriptor::initSetLayout(
            m_materialDescriptor.setLayout,
            m_internals.logicalDevice,
            materialLayoutBidings
        );
    }

    void LevelLayer::destroyMaterialDescriptorSetLayout()
    {
        m_internals.logicalDevice.destroyDescriptorSetLayout(
            m_materialDescriptor.setLayout
        );
    }

    void LevelLayer::initGraphicsPipeline()
    {
        if (!m_isInitialized)
        {
            return;
        }

        GraphicsPipeline::CreateInfo createInfo {};
        createInfo.canOverwrite          = false;
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
        createInfo.descriptorSetLayouts  = {
            m_frameDescriptor.setLayout,
            m_materialDescriptor.setLayout
        };

        m_graphicsPipeline = std::make_unique<GraphicsPipeline::Instance>(createInfo);
    }

    void LevelLayer::initFramebuffers()
    {
        if (!m_isInitialized)
        {
            return;
        }

        for (Frame::Instance& frame : m_internals.swapchain->images)
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

    void LevelLayer::initFrameResources()
    {
        if (!m_isInitialized)
        {
            return;
        }

        Descriptor::PoolCreateInfo descriptorPoolCreateInfo;
        descriptorPoolCreateInfo.size  = static_cast<uint32_t>(m_internals.swapchain->images.size());
        descriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eUniformBuffer);
        descriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eStorageBuffer);

        Descriptor::initPool(
            m_frameDescriptor.pool,
            m_internals.logicalDevice,
            descriptorPoolCreateInfo
        );

        for (Frame::Instance& frame : m_internals.swapchain->images)
        {
            vk::DescriptorSet sceneDescriptorSet;
            Descriptor::initSet(
                sceneDescriptorSet,
                m_internals.logicalDevice,
                m_frameDescriptor.setLayout,
                m_frameDescriptor.pool
            );
            frame.addDescriptorSet(m_id, sceneDescriptorSet);
    
            vk::WriteDescriptorSet cameraWriteInfo;
            cameraWriteInfo.dstSet          = sceneDescriptorSet;
            cameraWriteInfo.dstBinding      = 0;
            cameraWriteInfo.dstArrayElement = 0;
            cameraWriteInfo.descriptorCount = 1;
            cameraWriteInfo.descriptorType  = vk::DescriptorType::eUniformBuffer;
            cameraWriteInfo.pBufferInfo     = &frame.cameraDescriptorBufferInfo;
            frame.addWriteDescriptorSet(cameraWriteInfo);

            vk::WriteDescriptorSet modelWriteInfo;
            modelWriteInfo.dstSet          = sceneDescriptorSet;
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
        m_internals.logicalDevice.destroyDescriptorPool(
            m_frameDescriptor.pool
        );
    }

    void LevelLayer::initTextures()
    {
        if (!m_isInitialized)
        {
            return;
        }

        Descriptor::PoolCreateInfo descriptorPoolCreateInfo;
        descriptorPoolCreateInfo.size = 1000;
        descriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eCombinedImageSampler);

        Descriptor::initPool(
            m_materialDescriptor.pool,
            m_internals.logicalDevice,
            descriptorPoolCreateInfo
        );
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
            m_materialDescriptor.setLayout,
            m_materialDescriptor.pool
        );
    }

    void LevelLayer::destroyTextures()
    {
        m_internals.logicalDevice.destroyDescriptorPool(
            m_materialDescriptor.pool
        );
    }

    void LevelLayer::initMeshes()
    {
        if (!m_isInitialized)
        {
            return;
        }


        for (Frame::Instance& frame : m_internals.swapchain->images)
        {
            frame.deleteModelData();
            frame.setupModelData(m_level);
        }
    }

    void LevelLayer::buildMeshes()
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

    void LevelLayer::destroyMeshes()
    {
        Buffer::destroy(
            m_internals.logicalDevice,
            m_modelVertexBuffer
        );
        Buffer::destroy(
            m_internals.logicalDevice,
            m_modelIndexBuffer
        );
    }

    void LevelLayer::tickActors(float inDeltaTime)
    {
        if (!m_isInitialized)
        {
            return;
        }

        for (Actor* actor : m_level->getActors())
        {
            if (!actor->canTick())
            {
                continue;
            }

            actor->onTick(inDeltaTime);
        }
    }

    void LevelLayer::tickComponents(float inDeltaTime)
    {
        if (!m_isInitialized)
        {
            return;
        }

        for (ActorComponent* component : getComponents())
        {
            if (!component->canTick())
            {
                continue;
            }

            component->tick(inDeltaTime);
        }
    }

    void LevelLayer::tickAll()
    {
        if (!m_isInitialized)
        {
            return;
        }

        float deltaTime = getTelemetry().deltaToTick();

        tickActors(deltaTime);
        tickComponents(deltaTime);
    }
}