#include "Chicane/Core/Layer/SkyboxLayer.hpp"

#include "Chicane/Core.hpp"

namespace Chicane
{
    SkyboxLayer::SkyboxLayer(Window::Instance* inWindow)
        : Layer("Skybox"),
        m_clearValues({})
    {
        m_bIsInitialized     = Loader::getCubemapManager()->getCount() > 0;
        m_rendererInternals = inWindow->getRendererInternals();

        m_clearValues.push_back(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 0.0f));
    }

    SkyboxLayer::~SkyboxLayer()
    {
        m_rendererInternals.logicalDevice.waitIdle();

        // Graphics Pipeline
        m_graphicsPipeline.reset();

        // Descriptors
        m_rendererInternals.logicalDevice.destroyDescriptorSetLayout(
            m_frameDescriptor.setLayout
        );

        m_rendererInternals.logicalDevice.destroyDescriptorSetLayout(
            m_materialDescriptor.setLayout
        );
        m_rendererInternals.logicalDevice.destroyDescriptorPool(
            m_materialDescriptor.pool
        );
    }

    void SkyboxLayer::build()
    {
        if (!m_bIsInitialized)
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
    }

    void SkyboxLayer::destroy()
    {
        if (!m_bIsInitialized)
        {
            return;
        }

        m_rendererInternals.logicalDevice.destroyDescriptorPool(
            m_frameDescriptor.pool
        );
    }

    void SkyboxLayer::rebuild()
    {
        if (!m_bIsInitialized)
        {
            return;
        }

        initFramebuffers();
        initFrameResources();
    }

    void SkyboxLayer::render(
        Frame::Instance& outFrame,
        const vk::CommandBuffer& inCommandBuffer,
        const vk::Extent2D& inSwapChainExtent
    )
    {
        if (!m_bIsInitialized)
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

    void SkyboxLayer::initFrameDescriptorSetLayout()
    {
        Descriptor::SetLayoutBidingsCreateInfo frameLayoutBidings;
        frameLayoutBidings.count = 1;

        /// Camera
        frameLayoutBidings.indices.push_back(0);
        frameLayoutBidings.types.push_back(vk::DescriptorType::eUniformBuffer);
        frameLayoutBidings.counts.push_back(1);
        frameLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

        Descriptor::initSetLayout(
            m_frameDescriptor.setLayout,
            m_rendererInternals.logicalDevice,
            frameLayoutBidings
        );
    }

    void SkyboxLayer::initMaterialDescriptorSetLayout()
    {
        Descriptor::SetLayoutBidingsCreateInfo materialLayoutBidings;
        materialLayoutBidings.count = 1;
        materialLayoutBidings.indices.push_back(0);
        materialLayoutBidings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
        materialLayoutBidings.counts.push_back(1);
        materialLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eFragment);

        Descriptor::initSetLayout(
            m_materialDescriptor.setLayout,
            m_rendererInternals.logicalDevice,
            materialLayoutBidings
        );
    }

    void SkyboxLayer::initGraphicsPipeline()
    {
        GraphicsPipeline::Attachment colorAttachment {};
        colorAttachment.format        = m_rendererInternals.swapchain->format;
        colorAttachment.loadOp        = vk::AttachmentLoadOp::eClear;
        colorAttachment.initialLayout = vk::ImageLayout::eUndefined;

        GraphicsPipeline::CreateInfo createInfo {};
        createInfo.bHasVertices         = false;
        createInfo.bHasDepth            = false;
        createInfo.logicalDevice        = m_rendererInternals.logicalDevice;
        createInfo.vertexShaderPath     = "Content/Engine/Shaders/sky.vert.spv";
        createInfo.fragmentShaderPath   = "Content/Engine/Shaders/sky.frag.spv";
        createInfo.extent      = m_rendererInternals.swapchain->extent;
        createInfo.descriptorSetLayouts = { m_frameDescriptor.setLayout, m_materialDescriptor.setLayout };
        createInfo.colorAttachment      = colorAttachment;
        createInfo.polygonMode          = vk::PolygonMode::eFill;

        m_graphicsPipeline = std::make_unique<GraphicsPipeline::Instance>(createInfo);
    }

    void SkyboxLayer::initFramebuffers()
    {
        for (Frame::Instance& frame : m_rendererInternals.swapchain->frames)
        {
            Frame::Buffer::CreateInfo framebufferCreateInfo {};
            framebufferCreateInfo.id              = m_id;
            framebufferCreateInfo.logicalDevice   = m_rendererInternals.logicalDevice;
            framebufferCreateInfo.renderPass      = m_graphicsPipeline->renderPass;
            framebufferCreateInfo.swapChainExtent = m_rendererInternals.swapchain->extent;
            framebufferCreateInfo.attachments.push_back(frame.imageView);

            Frame::Buffer::init(frame, framebufferCreateInfo);
        }
    }

    void SkyboxLayer::initFrameResources()
    {
        Descriptor::PoolCreateInfo frameDescriptorPoolCreateInfo;
        frameDescriptorPoolCreateInfo.size = static_cast<uint32_t>(m_rendererInternals.swapchain->frames.size());
        frameDescriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eUniformBuffer);

        Descriptor::initPool(
            m_frameDescriptor.pool,
            m_rendererInternals.logicalDevice,
            frameDescriptorPoolCreateInfo
        );

        for (Frame::Instance& frame : m_rendererInternals.swapchain->frames)
        {
            vk::DescriptorSet descriptorSet;
            Descriptor::allocalteSet(
                descriptorSet,
                m_rendererInternals.logicalDevice,
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
        Descriptor::PoolCreateInfo materialDescriptorPoolCreateInfo;
        materialDescriptorPoolCreateInfo.size  = 1;
        materialDescriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eCombinedImageSampler);

        Descriptor::initPool(
            m_materialDescriptor.pool,
            m_rendererInternals.logicalDevice,
            materialDescriptorPoolCreateInfo
        );
    }

    void SkyboxLayer::buildAssets()
    {
        Loader::getCubemapManager()->build(
            m_rendererInternals.logicalDevice,
            m_rendererInternals.physicalDevice,
            m_rendererInternals.mainCommandBuffer,
            m_rendererInternals.graphicsQueue,
            m_materialDescriptor.setLayout,
            m_materialDescriptor.pool
        );
    }
}