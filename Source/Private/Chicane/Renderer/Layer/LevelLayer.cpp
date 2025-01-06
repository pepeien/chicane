#include "Chicane/Renderer/Layer/LevelLayer.hpp"

#include "Chicane/Application.hpp"
#include "Chicane/Box.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Game/Transformable/Component/MeshComponent.hpp"

namespace Chicane
{
    LevelLayer::LevelLayer()
        : Layer("Level"),
        m_internals(Application::getRenderer<Vulkan::Renderer>()->getInternals()),
        m_clearValues({}),
        m_textureManager(Loader::getTextureManager()),
        m_modelManager(Loader::getModelManager())
    {
        m_clearValues.push_back(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f));
        m_clearValues.push_back(vk::ClearDepthStencilValue(1.0f, 0));

        loadEvents();
    }

    LevelLayer::~LevelLayer()
    {
        m_internals.logicalDevice.waitIdle();

        destroyTextureResources();
        destroyModelData();

        m_graphicsPipeline.reset();
    }

    void LevelLayer::build()
    {
        if (!is(Status::Initialized))
        {
            return;
        }

        initFrameResources();
        initTextureResources();
        initGraphicsPipeline();
        initFramebuffers();

        buildTextures();
        buildModelData();
        setupFrames();

        setStatus(Status::Running);
    }

    void LevelLayer::destroy()
    {
        if (!is(Status::Running))
        {
            return;
        }

        destroyFrameResources();

        setStatus(Status::Idle);
    }

    void LevelLayer::rebuild()
    {
        if (!is(Status::Idle))
        {
            return;
        }

        initFrameResources();
        initFramebuffers();
        setupFrames();

        setStatus(Status::Running);
    }

    void LevelLayer::setup(Vulkan::Frame::Instance& outFrame)
    {
        if (!is(Status::Running))
        {
            return;
        }

        outFrame.updateMeshData(m_meshes);
    }

    void LevelLayer::render(
        Vulkan::Frame::Instance& outFrame,
        const vk::CommandBuffer& inCommandBuffer,
        const vk::Extent2D& inSwapChainExtent
    )
    {
        if (!is(Status::Running))
        {
            return;
        }

        vk::RenderPassBeginInfo renderPassBeginInfo {};
        renderPassBeginInfo.renderPass          = m_graphicsPipeline->renderPass;
        renderPassBeginInfo.framebuffer         = outFrame.getFramebuffer(m_id);
        renderPassBeginInfo.renderArea.offset.x = 0;
        renderPassBeginInfo.renderArea.offset.y = 0;
        renderPassBeginInfo.renderArea.extent   = inSwapChainExtent;
        renderPassBeginInfo.clearValueCount     = static_cast<std::uint32_t>(m_clearValues.size());
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
        if (!is(Status::Idle))
        {
            return;
        }

        Application::watchLevel(
            [this](Level* inLevel) {
                m_level = inLevel;

                if (!inLevel)
                {
                    return;
                }

                m_level->watchComponents(
                    [this](const std::vector<Component*>& inComponents) {
                        updateMeshes(inComponents);
                    }
                );
            }
        );

        m_textureManager->watchChanges(
            [&](Manager::EventType inEvent)
            {
                if (is(Status::Idle) && !m_modelManager->isEmpty())
                {
                    setStatus(Status::Initialized);

                    build();

                    return;
                }

                if (is(Status::Running))
                {
                    buildTextures();
                }
            }
        );

        m_modelManager->watchChanges(
            [&](Manager::EventType inEvent)
            {
                if (inEvent != Manager::EventType::Allocation)
                {
                    return;
                }

                if (is(Status::Idle) && !m_textureManager->isEmpty())
                {
                    setStatus(Status::Initialized);

                    build();

                    return;
                }

                if (is(Status::Running))
                {
                    rebuildModelData();

                    return;
                }
            }
        );
    }

    void LevelLayer::initFrameResources()
    {
        if (is(Status::Running))
        {
            return;
        }

        Vulkan::Descriptor::SetLayoutBidingsCreateInfo frameLayoutBidings;
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

        Vulkan::Descriptor::initSetLayout(
            m_frameDescriptor.setLayout,
            m_internals.logicalDevice,
            frameLayoutBidings
        );

        Vulkan::Descriptor::PoolCreateInfo descriptorPoolCreateInfo;
        descriptorPoolCreateInfo.size  = static_cast<std::uint32_t>(m_internals.swapchain->frames.size());
        descriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eUniformBuffer);
        descriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eStorageBuffer);

        Vulkan::Descriptor::initPool(
            m_frameDescriptor.pool,
            m_internals.logicalDevice,
            descriptorPoolCreateInfo
        );

        for (Vulkan::Frame::Instance& frame : m_internals.swapchain->frames)
        {
            vk::DescriptorSet levelDescriptorSet;
            Vulkan::Descriptor::allocalteSet(
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
        m_internals.logicalDevice.destroyDescriptorSetLayout(
            m_frameDescriptor.setLayout
        );
        m_internals.logicalDevice.destroyDescriptorPool(
            m_frameDescriptor.pool
        );
    }

    void LevelLayer::initTextureResources()
    {
        if (!is(Status::Initialized))
        {
            return;
        }

        Vulkan::Descriptor::SetLayoutBidingsCreateInfo materialLayoutBidings;
        materialLayoutBidings.count = 1;

        materialLayoutBidings.indices.push_back(0);
        materialLayoutBidings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
        materialLayoutBidings.counts.push_back(m_textureManager->getCount());
        materialLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eFragment);

        Vulkan::Descriptor::initSetLayout(
            m_textureDescriptor.setLayout,
            m_internals.logicalDevice,
            materialLayoutBidings
        );

        Vulkan::Descriptor::PoolCreateInfo descriptorPoolCreateInfo;
        descriptorPoolCreateInfo.size = TEXTURE_MAX_COUNT;
        descriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eCombinedImageSampler);

        Vulkan::Descriptor::initPool(
            m_textureDescriptor.pool,
            m_internals.logicalDevice,
            descriptorPoolCreateInfo
        );
    }

    void LevelLayer::destroyTextureResources()
    {
        m_internals.logicalDevice.destroyDescriptorSetLayout(
            m_textureDescriptor.setLayout
        );

        m_internals.logicalDevice.destroyDescriptorPool(
            m_textureDescriptor.pool
        );
    }

    void LevelLayer::initGraphicsPipeline()
    {
        if (!is(Status::Initialized))
        {
            return;
        }

        Vulkan::GraphicsPipeline::Attachment colorAttachment {};
        colorAttachment.format        = m_internals.swapchain->format;
        colorAttachment.loadOp        = vk::AttachmentLoadOp::eLoad;
        colorAttachment.initialLayout = vk::ImageLayout::ePresentSrcKHR;

        Vulkan::GraphicsPipeline::Attachment depthAttachment {};
        depthAttachment.format        = m_internals.swapchain->depthFormat;
        depthAttachment.loadOp        = vk::AttachmentLoadOp::eClear;
        depthAttachment.initialLayout = vk::ImageLayout::eUndefined;

        Vulkan::GraphicsPipeline::CreateInfo createInfo {};
        createInfo.bHasVertices          = true;
        createInfo.bHasDepth             = true;
        createInfo.logicalDevice         = m_internals.logicalDevice;
        createInfo.vertexShaderPath      = "Content/Engine/Shaders/level.vert.spv";
        createInfo.fragmentShaderPath    = "Content/Engine/Shaders/level.frag.spv";
        createInfo.bindingDescription    = Vulkan::Vertex::getBindingDescription();
        createInfo.attributeDescriptions = Vulkan::Vertex::getAttributeDescriptions();
        createInfo.extent                = m_internals.swapchain->extent;
        createInfo.descriptorSetLayouts  = { m_frameDescriptor.setLayout, m_textureDescriptor.setLayout };
        createInfo.colorAttachment       = colorAttachment;
        createInfo.depthAttachment       = depthAttachment;
        createInfo.polygonMode           = vk::PolygonMode::eFill;

        m_graphicsPipeline = std::make_unique<Vulkan::GraphicsPipeline::Instance>(createInfo);
    }

    void LevelLayer::initFramebuffers()
    {
        if (is(Status::Running))
        {
            return;
        }

        for (Vulkan::Frame::Instance& frame : m_internals.swapchain->frames)
        {
            Vulkan::Frame::Buffer::CreateInfo framebufferCreateInfo {};
            framebufferCreateInfo.id              = m_id;
            framebufferCreateInfo.logicalDevice   = m_internals.logicalDevice;
            framebufferCreateInfo.renderPass      = m_graphicsPipeline->renderPass;
            framebufferCreateInfo.swapChainExtent = m_internals.swapchain->extent;
            framebufferCreateInfo.attachments.push_back(frame.imageView);
            framebufferCreateInfo.attachments.push_back(frame.depth.imageView);

            Vulkan::Frame::Buffer::init(frame, framebufferCreateInfo);
        }
    }

    void LevelLayer::buildTextures()
    {
        m_textureManager->build(
            m_internals.logicalDevice,
            m_internals.physicalDevice,
            m_internals.mainCommandBuffer,
            m_internals.graphicsQueue,
            m_textureDescriptor
        );
    }

    void LevelLayer::buildModelData()
    {
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
        m_internals.logicalDevice.waitIdle();

        Vulkan::Buffer::destroy(
            m_internals.logicalDevice,
            m_modelVertexBuffer
        );
        Vulkan::Buffer::destroy(
            m_internals.logicalDevice,
            m_modelIndexBuffer
        );
    }

    void LevelLayer::rebuildModelData()
    {
        destroyModelData();

        if (m_modelManager->isEmpty())
        {
            return;
        }

        buildModelData();
    }

    void LevelLayer::setupFrames()
    {
        if (is(Status::Running))
        {
            return;
        }

        for (Vulkan::Frame::Instance& frame : m_internals.swapchain->frames)
        {
            frame.setupMeshData(m_meshes);
        }
    }

    void LevelLayer::setFramesAsDirty()
    {
        if (!is(Status::Running))
        {
            return;
        }

        for (Vulkan::Frame::Instance& frame : m_internals.swapchain->frames)
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

        setFramesAsDirty();
    }
}