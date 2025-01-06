#include "Chicane/Renderer/Layer/SkyboxLayer.hpp"

#include "Chicane/Application.hpp"
#include "Chicane/Core.hpp"

namespace Chicane
{
    SkyboxLayer::SkyboxLayer()
        : Layer("Skybox"),
        m_internals(Application::getRenderer<Vulkan::Renderer>()->getInternals()),
        m_clearValues({})
    {
        m_clearValues.push_back(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 0.0f));

        loadEvents();
    }

    SkyboxLayer::~SkyboxLayer()
    {
        m_internals.logicalDevice.waitIdle();

        // Graphics Pipeline
        m_graphicsPipeline.reset();

        // Descriptors
        m_internals.logicalDevice.destroyDescriptorSetLayout(
            m_frameDescriptor.setLayout
        );

        m_internals.logicalDevice.destroyDescriptorSetLayout(
            m_materialDescriptor.setLayout
        );
        m_internals.logicalDevice.destroyDescriptorPool(
            m_materialDescriptor.pool
        );
    }

    void SkyboxLayer::build()
    {
        if (!is(Status::Initialized))
        {
            return;
        }

        initFrameDescriptorSetLayout();
        initMaterialDescriptorSetLayout();
        initGraphicsPipeline();
        initFramebuffers();
        initFrameResources();
        initMaterialResources();

        buildAssets();

        setStatus(Status::Running);
    }

    void SkyboxLayer::destroy()
    {
        if (!is(Status::Running))
        {
            return;
        }

        m_internals.logicalDevice.destroyDescriptorPool(
            m_frameDescriptor.pool
        );

        setStatus(Status::Idle);
    }

    void SkyboxLayer::rebuild()
    {
        if (!is(Status::Idle))
        {
            return;
        }

        initFramebuffers();
        initFrameResources();

        setStatus(Status::Running);
    }

    void SkyboxLayer::render(
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
            Loader::getCubemapManager()->bind(
                "Skybox",
                inCommandBuffer,
                m_graphicsPipeline->layout
            );
            Loader::getCubemapManager()->draw(
                "Skybox",
                inCommandBuffer
            );
        inCommandBuffer.endRenderPass();
    }

    void SkyboxLayer::loadEvents()
    {
        if (!is(Status::Idle))
        {
            return;
        }

        Loader::getCubemapManager()->watchChanges(
            [this](Manager::EventType inEvent) {
                if (inEvent != Manager::EventType::Load)
                {
                    return;
                }

                if (!is(Status::Idle))
                {
                    return;
                }

                setStatus(Status::Initialized);

                build();
            }
        );
    }

    void SkyboxLayer::initFrameDescriptorSetLayout()
    {
        if (!is(Status::Initialized))
        {
            return;
        }

        Vulkan::Descriptor::SetLayoutBidingsCreateInfo frameLayoutBidings;
        frameLayoutBidings.count = 1;

        /// Camera
        frameLayoutBidings.indices.push_back(0);
        frameLayoutBidings.types.push_back(vk::DescriptorType::eUniformBuffer);
        frameLayoutBidings.counts.push_back(1);
        frameLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

        Vulkan::Descriptor::initSetLayout(
            m_frameDescriptor.setLayout,
            m_internals.logicalDevice,
            frameLayoutBidings
        );
    }

    void SkyboxLayer::initMaterialDescriptorSetLayout()
    {
        if (!is(Status::Initialized))
        {
            return;
        }

        Vulkan::Descriptor::SetLayoutBidingsCreateInfo materialLayoutBidings;
        materialLayoutBidings.count = 1;
        materialLayoutBidings.indices.push_back(0);
        materialLayoutBidings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
        materialLayoutBidings.counts.push_back(1);
        materialLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eFragment);

        Vulkan::Descriptor::initSetLayout(
            m_materialDescriptor.setLayout,
            m_internals.logicalDevice,
            materialLayoutBidings
        );
    }

    void SkyboxLayer::initGraphicsPipeline()
    {
        if (!is(Status::Initialized))
        {
            return;
        }

        Vulkan::GraphicsPipeline::Attachment colorAttachment {};
        colorAttachment.format        = m_internals.swapchain->format;
        colorAttachment.loadOp        = vk::AttachmentLoadOp::eClear;
        colorAttachment.initialLayout = vk::ImageLayout::eUndefined;

        Vulkan::GraphicsPipeline::CreateInfo createInfo {};
        createInfo.bHasVertices         = false;
        createInfo.bHasDepth            = false;
        createInfo.logicalDevice        = m_internals.logicalDevice;
        createInfo.vertexShaderPath     = "Content/Engine/Shaders/sky.vert.spv";
        createInfo.fragmentShaderPath   = "Content/Engine/Shaders/sky.frag.spv";
        createInfo.extent      = m_internals.swapchain->extent;
        createInfo.descriptorSetLayouts = { m_frameDescriptor.setLayout, m_materialDescriptor.setLayout };
        createInfo.colorAttachment      = colorAttachment;
        createInfo.polygonMode          = vk::PolygonMode::eFill;

        m_graphicsPipeline = std::make_unique<Vulkan::GraphicsPipeline::Instance>(createInfo);
    }

    void SkyboxLayer::initFramebuffers()
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

            Vulkan::Frame::Buffer::init(frame, framebufferCreateInfo);
        }
    }

    void SkyboxLayer::initFrameResources()
    {
        if (is(Status::Running))
        {
            return;
        }

        Vulkan::Descriptor::PoolCreateInfo frameDescriptorPoolCreateInfo;
        frameDescriptorPoolCreateInfo.size = static_cast<std::uint32_t>(m_internals.swapchain->frames.size());
        frameDescriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eUniformBuffer);

        Vulkan::Descriptor::initPool(
            m_frameDescriptor.pool,
            m_internals.logicalDevice,
            frameDescriptorPoolCreateInfo
        );

        for (Vulkan::Frame::Instance& frame : m_internals.swapchain->frames)
        {
            vk::DescriptorSet descriptorSet;
            Vulkan::Descriptor::allocalteSet(
                descriptorSet,
                m_internals.logicalDevice,
                m_frameDescriptor.setLayout,
                m_frameDescriptor.pool
            );
            frame.addDescriptorSet(m_id, descriptorSet);

            vk::WriteDescriptorSet writeDescriptorSet;
            writeDescriptorSet.dstSet          = descriptorSet;
            writeDescriptorSet.dstBinding      = 0;
            writeDescriptorSet.dstArrayElement = 0;
            writeDescriptorSet.descriptorCount = 1;
            writeDescriptorSet.descriptorType  = vk::DescriptorType::eUniformBuffer;
            writeDescriptorSet.pBufferInfo     = &frame.cameraDescriptorBufferInfo;
            frame.addWriteDescriptorSet(writeDescriptorSet);
        }
    }

    void SkyboxLayer::initMaterialResources()
    {
        if (!is(Status::Initialized))
        {
            return;
        }

        Vulkan::Descriptor::PoolCreateInfo materialDescriptorPoolCreateInfo;
        materialDescriptorPoolCreateInfo.size  = 1;
        materialDescriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eCombinedImageSampler);

        Vulkan::Descriptor::initPool(
            m_materialDescriptor.pool,
            m_internals.logicalDevice,
            materialDescriptorPoolCreateInfo
        );
    }

    void SkyboxLayer::buildAssets()
    {
        if (!is(Status::Initialized))
        {
            return;
        }

        Loader::getCubemapManager()->build(
            m_internals.logicalDevice,
            m_internals.physicalDevice,
            m_internals.mainCommandBuffer,
            m_internals.graphicsQueue,
            m_materialDescriptor.setLayout,
            m_materialDescriptor.pool
        );
    }
}