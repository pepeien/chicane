#include "Runtime/Core/Layers/Level.hpp"

#include "Runtime/Core/Log.hpp"
#include "Runtime/Core/Window.hpp"

namespace Chicane
{
    LevelLayer::LevelLayer(Window* inWindow)
        : Layer("Level"),
        m_renderer(inWindow->getRenderer()),
        m_level(inWindow->getLevel()),
        m_meshManager(std::make_unique<Mesh::Manager::Instance>()),
        m_textureManager(std::make_unique<Texture::Manager::Instance>())
    {
        m_isInitialized = (nullptr != m_level) && m_level->hasActors();
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
            m_meshVertexBuffer
        );
        Buffer::destroy(
            m_renderer->m_logicalDevice,
            m_meshIndexBuffer
        );

        // Managers
        m_meshManager.reset();
        m_textureManager.reset();
    }

    void LevelLayer::build()
    {
        if (!m_isInitialized)
        {
            return;
        }

        loadEvents();
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
    }

    void LevelLayer::setup(Frame::Instance& outFrame)
    {
        if (!m_isInitialized)
        {
            return;
        }

        outFrame.updateModelData(m_level->getActors());

        memcpy(
            outFrame.meshData.writeLocation,
            outFrame.meshData.transforms.data(),
            outFrame.meshData.allocationSize
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
        clearValues.push_back(vk::ClearColorValue(1.0f, 1.0f, 1.0f, 1.0f));
        clearValues.push_back(vk::ClearDepthStencilValue(1.0f, 0));

        vk::RenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.renderPass          = m_graphicsPipeline->renderPass;
        renderPassBeginInfo.framebuffer         = outFrame.getFramebuffer(m_name);
        renderPassBeginInfo.renderArea.offset.x = 0;
        renderPassBeginInfo.renderArea.offset.y = 0;
        renderPassBeginInfo.renderArea.extent   = inSwapChainExtent;
        renderPassBeginInfo.clearValueCount     = static_cast<uint32_t>(clearValues.size());
        renderPassBeginInfo.pClearValues        = clearValues.data();

        inCommandBuffer.beginRenderPass(
            &renderPassBeginInfo,
            vk::SubpassContents::eInline
        );

        // Viewport
        m_renderer->updateViewport(inCommandBuffer);

        // Preparing
        inCommandBuffer.bindPipeline(
            vk::PipelineBindPoint::eGraphics,
            m_graphicsPipeline->instance
        );

        inCommandBuffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            m_graphicsPipeline->layout,
            0,
            outFrame.getDescriptorSet(m_name),
            nullptr
        );

        vk::Buffer vertexBuffers[] = { m_meshVertexBuffer.instance };
        vk::DeviceSize offsets[]   = { 0 };

        inCommandBuffer.bindVertexBuffers(
            0,
            1,
            vertexBuffers,
            offsets
        );

        inCommandBuffer.bindIndexBuffer(
            m_meshIndexBuffer.instance,
            0,
            vk::IndexType::eUint32
        );

        m_textureManager->bindAll(
            inCommandBuffer,
            m_graphicsPipeline->layout
        );

        // Drawing
        m_meshManager->drawAll(inCommandBuffer);

        inCommandBuffer.endRenderPass();

        return;
    }

    void LevelLayer::loadActor(Actor* inActor)
    {
        if (!inActor->hasModel())
        {
            return;
        }

        Box::Instance mesh = inActor->getModel();

        for (Box::Entry meshComponent : mesh.entries)
        {
            if (meshComponent.type == static_cast<uint8_t>(Box::EntryType::Mesh))
            {
                m_meshManager->add(
                    mesh.name,
                    meshComponent.data,
                    Mesh::Vendor::Wavefront
                );

                continue;
            }

            if (meshComponent.type == static_cast<uint8_t>(Box::EntryType::Texture))
            {
                m_textureManager->add(
                    mesh.name,
                    meshComponent.data
                );

                continue;
            }
        }
    }

    void LevelLayer::loadAssets()
    {
        if (nullptr == m_level)
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
        GraphicsPipeline::CreateInfo createInfo = {};
        createInfo.canOverwrite          = true;
        createInfo.hasVertices           = true;
        createInfo.hasDepth              = true;
        createInfo.logicalDevice         = m_renderer->m_logicalDevice;
        createInfo.vertexShaderPath      = "Shaders/triangle.vert.spv";
        createInfo.fragmentShaderPath    = "Shaders/triangle.frag.spv";
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
        for (Frame::Instance& frame : m_renderer->m_swapChain.images)
        {
            Frame::Buffer::CreateInfo framebufferCreateInfo = {};
            framebufferCreateInfo.id              = m_name;
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
            // Data
            frame.setupCameraMatrixUBO();
            frame.setupModelData(m_level->getActors());

            // Scene
            vk::DescriptorSet sceneDescriptorSet;
            Descriptor::initSet(
                sceneDescriptorSet,
                m_renderer->m_logicalDevice,
                m_frameDescriptor.setLayout,
                m_frameDescriptor.pool
            );
            frame.addDescriptorSet(m_name, sceneDescriptorSet);
    
            vk::WriteDescriptorSet cameraMatrixWriteInfo;
            cameraMatrixWriteInfo.dstSet          = sceneDescriptorSet;
            cameraMatrixWriteInfo.dstBinding      = 0;
            cameraMatrixWriteInfo.dstArrayElement = 0;
            cameraMatrixWriteInfo.descriptorCount = 1;
            cameraMatrixWriteInfo.descriptorType  = vk::DescriptorType::eUniformBuffer;
            cameraMatrixWriteInfo.pBufferInfo     = &frame.cameraMatrixDescriptorBufferInfo;
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
        Descriptor::PoolCreateInfo descriptorPoolCreateInfo;
        descriptorPoolCreateInfo.size = m_textureManager->getCount();
        descriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eCombinedImageSampler);

        Descriptor::initPool(
            m_materialDescriptor.pool,
            m_renderer->m_logicalDevice,
            descriptorPoolCreateInfo
        );
    }

    void LevelLayer::buildAssets()
    {
        m_meshManager->build(
            m_meshVertexBuffer,
            m_meshIndexBuffer,
            m_renderer->m_logicalDevice,
            m_renderer->m_physicalDevice,
            m_renderer->m_graphicsQueue,
            m_renderer->m_mainCommandBuffer
        );

        m_textureManager->build(
            m_renderer->m_logicalDevice,
            m_renderer->m_physicalDevice,
            m_renderer->m_mainCommandBuffer,
            m_renderer->m_graphicsQueue,
            m_materialDescriptor.setLayout,
            m_materialDescriptor.pool
        );
    }

    void LevelLayer::loadEvents()
    {
        if (nullptr == m_level)
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
}