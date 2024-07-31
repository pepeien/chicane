#include "Chicane/Core/Layers/Level.hpp"

#include "Chicane/Core.hpp"

namespace Chicane
{
    LevelLayer::LevelLayer(Window* inWindow)
        : Layer("Level"),
        m_renderer(inWindow->getRenderer())
    {
        if (hasLevel() == false)
        {
            return;
        }

        m_level         = getLevel();
        m_isInitialized = m_level->hasActors();
    }

    LevelLayer::~LevelLayer()
    {
        m_renderer->m_logicalDevice.waitIdle();

        // Graphics Pipeline
        m_graphicsPipeline.reset();

        // Descriptors
        m_renderer->m_logicalDevice.destroyDescriptorSetLayout(
            m_frameDescriptor.setLayout
        );

        m_renderer->m_logicalDevice.destroyDescriptorSetLayout(
            m_materialDescriptor.setLayout
        );
        m_renderer->m_logicalDevice.destroyDescriptorPool(
            m_materialDescriptor.pool
        );

        // Buffers
        Buffer::destroy(
            m_renderer->m_logicalDevice,
            m_modelVertexBuffer
        );
        Buffer::destroy(
            m_renderer->m_logicalDevice,
            m_modelIndexBuffer
        );
    }

    void LevelLayer::build()
    {
        loadEvents();

        if (!m_isInitialized)
        {
            return;
        }

        loadAssets();
        initFrameDescriptorSetLayout();
        initMaterialDescriptorSetLayout();
        initGraphicsPipeline();
        initFramebuffers();
        initFrameResources();
        initMaterialResources();
        buildAssets();
    }

    void LevelLayer::destroy()
    {
        if (!m_isInitialized)
        {
            return;
        }

        m_renderer->m_logicalDevice.destroyDescriptorPool(
            m_frameDescriptor.pool
        );
    }

    void LevelLayer::rebuild()
    {
        if (!m_isInitialized)
        {
            return;
        }

        initFramebuffers();
        initFrameResources();
        loadAssets();
    }

    void LevelLayer::setup(Frame::Instance& outFrame)
    {
        if (!m_isInitialized)
        {
            return;
        }

        outFrame.updateModelData(m_level->getActors());

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

        Allocator::getModelManager()->drawAll(
            inCommandBuffer
        );

        inCommandBuffer.endRenderPass();

        tickActors();
    }

    void LevelLayer::loadEvents()
    {
        if (!m_isInitialized)
        {
            return;
        }

        m_level->addActorSubscription(
            std::bind(
                &LevelLayer::loadActor,
                this,
                std::placeholders::_1
            )
        );
    }

    void LevelLayer::loadActor(Actor* inActor)
    {
        if (!m_isInitialized)
        {
            m_isInitialized = true;

            build();
        }

        for (Frame::Instance& frame : m_renderer->m_swapChain.images)
        {
            frame.setupModelData(m_level->getActors());
        }
    }

    void LevelLayer::loadAssets()
    {
        if (!m_isInitialized)
        {
            return;
        }

        for (Actor* actor : m_level->getActors())
        {
            loadActor(actor);
        }
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
            m_renderer->m_logicalDevice,
            frameLayoutBidings
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
            m_renderer-> m_logicalDevice,
            materialLayoutBidings
        );
    }

    void LevelLayer::initGraphicsPipeline()
    {
        if (!m_isInitialized)
        {
            return;
        }

        GraphicsPipeline::CreateInfo createInfo {};
        createInfo.canOverwrite          = true;
        createInfo.hasVertices           = true;
        createInfo.hasDepth              = true;
        createInfo.logicalDevice         = m_renderer->m_logicalDevice;
        createInfo.vertexShaderPath      = "Content/Engine/Shaders/level.vert.spv";
        createInfo.fragmentShaderPath    = "Content/Engine/Shaders/level.frag.spv";
        createInfo.bindingDescription    = Vertex::getBindingDescription();
        createInfo.attributeDescriptions = Vertex::getAttributeDescriptions();
        createInfo.swapChainExtent       = m_renderer->m_swapChain.extent;
        createInfo.swapChainImageFormat  = m_renderer->m_swapChain.format;
        createInfo.depthFormat           = m_renderer->m_swapChain.images[0].depthFormat;
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

        for (Frame::Instance& frame : m_renderer->m_swapChain.images)
        {
            Frame::Buffer::CreateInfo framebufferCreateInfo {};
            framebufferCreateInfo.id              = m_id;
            framebufferCreateInfo.logicalDevice   = m_renderer->m_logicalDevice;
            framebufferCreateInfo.renderPass      = m_graphicsPipeline->renderPass;
            framebufferCreateInfo.swapChainExtent = m_renderer->m_swapChain.extent;
            framebufferCreateInfo.attachments.push_back(frame.imageView);
            framebufferCreateInfo.attachments.push_back(frame.depthImageView);

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
        descriptorPoolCreateInfo.size  = static_cast<uint32_t>(m_renderer->m_swapChain.images.size());
        descriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eUniformBuffer);
        descriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eStorageBuffer);

        Descriptor::initPool(
            m_frameDescriptor.pool,
            m_renderer->m_logicalDevice,
            descriptorPoolCreateInfo
        );

        for (Frame::Instance& frame : m_renderer->m_swapChain.images)
        {
            // Scene
            vk::DescriptorSet sceneDescriptorSet;
            Descriptor::initSet(
                sceneDescriptorSet,
                m_renderer->m_logicalDevice,
                m_frameDescriptor.setLayout,
                m_frameDescriptor.pool
            );
            frame.addDescriptorSet(m_id, sceneDescriptorSet);
    
            vk::WriteDescriptorSet cameraMatrixWriteInfo;
            cameraMatrixWriteInfo.dstSet          = sceneDescriptorSet;
            cameraMatrixWriteInfo.dstBinding      = 0;
            cameraMatrixWriteInfo.dstArrayElement = 0;
            cameraMatrixWriteInfo.descriptorCount = 1;
            cameraMatrixWriteInfo.descriptorType  = vk::DescriptorType::eUniformBuffer;
            cameraMatrixWriteInfo.pBufferInfo     = &frame.cameraDescriptorBufferInfo;
            frame.addWriteDescriptorSet(cameraMatrixWriteInfo);

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

    void LevelLayer::initMaterialResources()
    {
        if (!m_isInitialized)
        {
            return;
        }

        Descriptor::PoolCreateInfo descriptorPoolCreateInfo;
        descriptorPoolCreateInfo.size = Allocator::getTextureManager()->getCount();
        descriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eCombinedImageSampler);

        Descriptor::initPool(
            m_materialDescriptor.pool,
            m_renderer->m_logicalDevice,
            descriptorPoolCreateInfo
        );
    }

    void LevelLayer::buildAssets()
    {
        if (!m_isInitialized)
        {
            return;
        }

        Allocator::getModelManager()->build(
            m_modelVertexBuffer,
            m_modelIndexBuffer,
            m_renderer->m_logicalDevice,
            m_renderer->m_physicalDevice,
            m_renderer->m_graphicsQueue,
            m_renderer->m_mainCommandBuffer
        );

        Allocator::getTextureManager()->build(
            m_renderer->m_logicalDevice,
            m_renderer->m_physicalDevice,
            m_renderer->m_mainCommandBuffer,
            m_renderer->m_graphicsQueue,
            m_materialDescriptor.setLayout,
            m_materialDescriptor.pool
        );
    }

    void LevelLayer::tickActors()
    {
        if (!m_isInitialized)
        {
            return;
        }

        for (Actor* actor : m_level->getActors())
        {
            if (actor->canTick() == false)
            {
                continue;
            }

            actor->onTick(getTelemetry().deltaToTick());
        }
    }
}