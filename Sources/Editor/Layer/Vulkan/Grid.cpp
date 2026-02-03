#include "Layer/Vulkan/Grid.hpp"

#include "Chicane/Renderer/Backend/Vulkan.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Data.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout/BidingsCreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/GraphicsPipeline/Builder.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Layer/Scene.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Vertex.hpp"

namespace Editor
{
    VulkanLGrid::VulkanLGrid()
        : Layer("Editor_Scene_Grid"),
          m_clear({vk::ClearColorValue(0.0f, 0.0f, 0.0f, 0.0f), vk::ClearDepthStencilValue(1.0f, 0)})
    {}

    bool VulkanLGrid::onInit()
    {
        initFrameResources();

        initGraphicsPipeline();
        initFramebuffers();

        return true;
    }

    bool VulkanLGrid::onDestroy()
    {
        destroyFrameResources();

        return true;
    }

    bool VulkanLGrid::onRebuild()
    {
        initFrameResources();
        initFramebuffers();

        return true;
    }

    void VulkanLGrid::onRender(const Chicane::Renderer::Frame& inFrame, void* inData)
    {
        Chicane::Renderer::VulkanBackend* backend = getBackend<Chicane::Renderer::VulkanBackend>();

        Chicane::Renderer::VulkanBackendData* data          = (Chicane::Renderer::VulkanBackendData*)inData;
        vk::CommandBuffer&                    commandBuffer = data->commandBuffer;
        Chicane::Renderer::VulkanFrame&       frame         = data->frame;

        vk::RenderPassBeginInfo beginInfo = {};
        beginInfo.renderPass              = m_graphicsPipeline.renderPass;
        beginInfo.framebuffer             = frame.getFramebuffer(m_id);
        beginInfo.renderArea.extent       = backend->swapchain.extent;
        beginInfo.clearValueCount         = static_cast<std::uint32_t>(m_clear.size());
        beginInfo.pClearValues            = m_clear.data();

        commandBuffer.beginRenderPass(&beginInfo, vk::SubpassContents::eInline);

        // Pipeline
        m_graphicsPipeline.bind(commandBuffer);

        // Frame
        m_graphicsPipeline.bindDescriptorSet(commandBuffer, 0, frame.getDescriptorSet(m_id));

        // Draw
        commandBuffer.draw(6, 1, 0, 0);

        commandBuffer.endRenderPass();
    }

    void VulkanLGrid::initFrameResources()
    {
        Chicane::Renderer::VulkanDescriptorSetLayoutBidingsCreateInfo bidings;
        bidings.count = 1;

        // Camera
        bidings.indices.push_back(0);
        bidings.types.push_back(vk::DescriptorType::eUniformBuffer);
        bidings.counts.push_back(1);
        bidings.stages.push_back(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);

        Chicane::Renderer::VulkanDescriptorSetLayout::init(
            m_frameDescriptor.setLayout,
            getBackend<Chicane::Renderer::VulkanBackend>()->logicalDevice,
            bidings
        );

        Chicane::Renderer::VulkanDescriptorPoolCreateInfo descriptorPoolCreateInfo;
        descriptorPoolCreateInfo.maxSets =
            static_cast<std::uint32_t>(getBackend<Chicane::Renderer::VulkanBackend>()->swapchain.frames.size());
        descriptorPoolCreateInfo.sizes.push_back(
            {vk::DescriptorType::eUniformBuffer, descriptorPoolCreateInfo.maxSets}
        );

        Chicane::Renderer::VulkanDescriptorPool::init(
            m_frameDescriptor.pool,
            getBackend<Chicane::Renderer::VulkanBackend>()->logicalDevice,
            descriptorPoolCreateInfo
        );

        for (Chicane::Renderer::VulkanFrame& frame : getBackend<Chicane::Renderer::VulkanBackend>()->swapchain.frames)
        {
            vk::DescriptorSet descriptorSet;

            Chicane::Renderer::VulkanDescriptorSetLayout::allocate(
                descriptorSet,
                getBackend<Chicane::Renderer::VulkanBackend>()->logicalDevice,
                m_frameDescriptor.setLayout,
                m_frameDescriptor.pool
            );
            frame.addDescriptorSet(m_id, descriptorSet);

            vk::WriteDescriptorSet cameraWriteInfo;
            cameraWriteInfo.dstSet          = descriptorSet;
            cameraWriteInfo.dstBinding      = 0;
            cameraWriteInfo.dstArrayElement = 0;
            cameraWriteInfo.descriptorCount = 1;
            cameraWriteInfo.descriptorType  = vk::DescriptorType::eUniformBuffer;
            cameraWriteInfo.pBufferInfo     = &frame.cameraResource.bufferInfo;
            frame.addWriteDescriptorSet(cameraWriteInfo);
        }
    }

    void VulkanLGrid::destroyFrameResources()
    {
        getBackend<Chicane::Renderer::VulkanBackend>()->logicalDevice.destroyDescriptorSetLayout(
            m_frameDescriptor.setLayout
        );
        getBackend<Chicane::Renderer::VulkanBackend>()->logicalDevice.destroyDescriptorPool(m_frameDescriptor.pool);
    }

    void VulkanLGrid::initGraphicsPipeline()
    {
        // Backend
        Chicane::Renderer::VulkanBackend* backend = getBackend<Chicane::Renderer::VulkanBackend>();

        // Shader
        Chicane::Renderer::VulkanShaderStageCreateInfo vertexShader;
        vertexShader.path = "Contents/Editor/Shaders/Vulkan/Grid.vvert";
        vertexShader.type = vk::ShaderStageFlagBits::eVertex;

        Chicane::Renderer::VulkanShaderStageCreateInfo fragmentShader;
        fragmentShader.path = "Contents/Editor/Shaders/Vulkan/Grid.vfrag";
        fragmentShader.type = vk::ShaderStageFlagBits::eFragment;

        // Depth
        vk::PipelineDepthStencilStateCreateInfo depth;
        depth.flags                 = vk::PipelineDepthStencilStateCreateFlags();
        depth.depthBoundsTestEnable = VK_FALSE;
        depth.stencilTestEnable     = VK_FALSE;
        depth.depthWriteEnable      = VK_TRUE;
        depth.depthTestEnable       = VK_TRUE;
        depth.depthCompareOp        = vk::CompareOp::eLessOrEqual;
        depth.minDepthBounds        = 0.0f;
        depth.maxDepthBounds        = 1.0f;

        // Render pass
        vk::AttachmentDescription colorAttachment;
        colorAttachment.flags         = vk::AttachmentDescriptionFlags();
        colorAttachment.format        = backend->swapchain.colorFormat;
        colorAttachment.samples       = vk::SampleCountFlagBits::e1;
        colorAttachment.loadOp        = vk::AttachmentLoadOp::eLoad;
        colorAttachment.storeOp       = vk::AttachmentStoreOp::eStore;
        colorAttachment.initialLayout = vk::ImageLayout::eColorAttachmentOptimal;
        colorAttachment.finalLayout   = vk::ImageLayout::eColorAttachmentOptimal;

        vk::AttachmentReference colorReference;
        colorReference.attachment = 0;
        colorReference.layout     = vk::ImageLayout::eColorAttachmentOptimal;

        vk::SubpassDependency colorSubpassDepedency;
        colorSubpassDepedency.srcSubpass    = 0;
        colorSubpassDepedency.dstSubpass    = VK_SUBPASS_EXTERNAL;
        colorSubpassDepedency.srcStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        colorSubpassDepedency.dstStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        colorSubpassDepedency.srcAccessMask = vk::AccessFlagBits::eNone;
        colorSubpassDepedency.dstAccessMask =
            vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;

        vk::AttachmentDescription depthAttachment;
        depthAttachment.flags         = vk::AttachmentDescriptionFlags();
        depthAttachment.format        = backend->swapchain.depthFormat;
        depthAttachment.samples       = vk::SampleCountFlagBits::e1;
        depthAttachment.loadOp        = vk::AttachmentLoadOp::eLoad;
        depthAttachment.storeOp       = vk::AttachmentStoreOp::eStore;
        depthAttachment.initialLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        depthAttachment.finalLayout   = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        vk::AttachmentReference depthReference;
        depthReference.attachment = 1;
        depthReference.layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        vk::SubpassDependency depthSubpassDepedency;
        depthSubpassDepedency.srcSubpass    = 0;
        depthSubpassDepedency.dstSubpass    = VK_SUBPASS_EXTERNAL;
        depthSubpassDepedency.srcStageMask  = vk::PipelineStageFlagBits::eLateFragmentTests;
        depthSubpassDepedency.dstStageMask  = vk::PipelineStageFlagBits::eFragmentShader;
        depthSubpassDepedency.srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        depthSubpassDepedency.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        vk::SubpassDescription subpass;
        subpass.flags                   = vk::SubpassDescriptionFlags();
        subpass.pipelineBindPoint       = vk::PipelineBindPoint::eGraphics;
        subpass.colorAttachmentCount    = 1;
        subpass.pColorAttachments       = &colorReference;
        subpass.pDepthStencilAttachment = &depthReference;

        // Rasterizer
        vk::PipelineRasterizationStateCreateInfo rasterization;
        rasterization.flags                   = vk::PipelineRasterizationStateCreateFlags();
        rasterization.depthClampEnable        = VK_FALSE;
        rasterization.rasterizerDiscardEnable = VK_FALSE;
        rasterization.lineWidth               = 1.0f;
        rasterization.depthBiasEnable         = VK_TRUE;
        rasterization.depthBiasClamp          = 0.0f;
        rasterization.polygonMode             = vk::PolygonMode::eFill;
        rasterization.cullMode                = vk::CullModeFlagBits::eBack;
        rasterization.frontFace               = vk::FrontFace::eCounterClockwise;

        // Build
        Chicane::Renderer::VulkanGraphicsPipelineBuilder()
            .setInputAssembly(Chicane::Renderer::VulkanGraphicsPipeline::createInputAssemblyState())
            .addViewport(backend->viewport)
            .addDynamicState(vk::DynamicState::eViewport)
            .addScissor(backend->scissor)
            .addDynamicState(vk::DynamicState::eScissor)
            .addShaderStage(vertexShader, backend->logicalDevice)
            .addShaderStage(fragmentShader, backend->logicalDevice)
            .addColorBlendingAttachment(Chicane::Renderer::VulkanGraphicsPipeline::createBlendAttachmentState(true))
            .addAttachment(colorAttachment)
            .addSubpassDependecy(colorSubpassDepedency)
            .setDepthStencil(depth)
            .addAttachment(depthAttachment)
            .addSubpassDependecy(depthSubpassDepedency)
            .addSubpass(subpass)
            .addDescriptorSetLayout(m_frameDescriptor.setLayout)
            .setRasterization(rasterization)
            .build(m_graphicsPipeline, backend->logicalDevice);
    }

    void VulkanLGrid::initFramebuffers()
    {
        for (Chicane::Renderer::VulkanFrame& frame : getBackend<Chicane::Renderer::VulkanBackend>()->swapchain.frames)
        {
            Chicane::Renderer::VulkanFrameCreateInfo createInfo;
            createInfo.id            = m_id;
            createInfo.logicalDevice = getBackend<Chicane::Renderer::VulkanBackend>()->logicalDevice;
            createInfo.renderPass    = m_graphicsPipeline.renderPass;
            createInfo.extent        = getBackend<Chicane::Renderer::VulkanBackend>()->swapchain.extent;
            createInfo.attachments.push_back(frame.colorImage.view);
            createInfo.attachments.push_back(frame.depthImage.view);

            frame.addBuffer(createInfo);
        }
    }
}