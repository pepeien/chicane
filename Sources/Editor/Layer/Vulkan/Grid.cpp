#include "Layer/Vulkan/Grid.hpp"

#include <Chicane/Renderer/Backend/Vulkan.hpp>
#include <Chicane/Renderer/Backend/Vulkan/Descriptor/Pool.hpp>
#include <Chicane/Renderer/Backend/Vulkan/Descriptor/Pool/CreateInfo.hpp>
#include <Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout.hpp>
#include <Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout/BidingsCreateInfo.hpp>
#include <Chicane/Renderer/Backend/Vulkan/Frame.hpp>
#include <Chicane/Renderer/Backend/Vulkan/GraphicsPipeline/Builder.hpp>
#include <Chicane/Renderer/Backend/Vulkan/Layer/Scene.hpp>
#include <Chicane/Renderer/Backend/Vulkan/Vertex.hpp>

namespace Editor
{
    VulkanLUI::VulkanLUI()
        : Layer("Editor_Scene_Grid"),
          m_clear({vk::ClearColorValue(0.0f, 0.0f, 0.0f, 0.0f), vk::ClearDepthStencilValue(1.0f, 0)})
    {}

    void VulkanLUI::onInit()
    {
        initFrameResources();

        initGraphicsPipeline();
        initFramebuffers();

        initViewport();
    }

    void VulkanLUI::onRestart()
    {
        initFramebuffers();
    }

    void VulkanLUI::onDestruction()
    {
        destroyFrameResources();

        m_graphicsPipeline.destroy();
    }

    void VulkanLUI::onRender(const Chicane::Renderer::Frame& inFrame, void* inData)
    {
        Chicane::Renderer::VulkanBackend* backend = getBackend<Chicane::Renderer::VulkanBackend>();

        Chicane::Renderer::VulkanFrame frame         = *((Chicane::Renderer::VulkanFrame*)inData);
        vk::CommandBuffer              commandBuffer = frame.commandBuffer;

        vk::Viewport viewport = backend->getVkViewport(this);
        commandBuffer.setViewport(0, 1, &viewport);

        vk::Rect2D scissor = backend->getVkScissor(this);
        commandBuffer.setScissor(0, 1, &scissor);

        vk::RenderPassBeginInfo beginInfo  = {};
        beginInfo.renderPass               = m_graphicsPipeline.renderPass;
        beginInfo.framebuffer              = frame.image.getFramebuffer(m_id);
        beginInfo.renderArea.extent.width  = viewport.width;
        beginInfo.renderArea.extent.height = viewport.height;
        beginInfo.clearValueCount          = static_cast<std::uint32_t>(m_clear.size());
        beginInfo.pClearValues             = m_clear.data();

        commandBuffer.beginRenderPass(&beginInfo, vk::SubpassContents::eInline);

        // Pipeline
        m_graphicsPipeline.bind(commandBuffer);

        // Frame
        m_graphicsPipeline.bind(commandBuffer, 0, frame.getDescriptorSet(m_id));

        // Draw
        commandBuffer.draw(6, 1, 0, 0);

        commandBuffer.endRenderPass();
    }

    void VulkanLUI::initFrameResources()
    {
        Chicane::Renderer::VulkanBackend* backend = getBackend<Chicane::Renderer::VulkanBackend>();

        Chicane::Renderer::VulkanDescriptorSetLayoutBidingsCreateInfo bidings;
        bidings.count = 1;

        // Camera
        bidings.indices.push_back(0);
        bidings.types.push_back(vk::DescriptorType::eUniformBuffer);
        bidings.counts.push_back(1);
        bidings.stages.push_back(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);

        Chicane::Renderer::VulkanDescriptorSetLayout::init(
            m_frameDescriptor.setLayout,
            backend->logicalDevice,
            bidings
        );

        Chicane::Renderer::VulkanDescriptorPoolCreateInfo descriptorPoolCreateInfo;
        descriptorPoolCreateInfo.maxSets = static_cast<std::uint32_t>(backend->frames.size());
        descriptorPoolCreateInfo.sizes.push_back(
            {vk::DescriptorType::eUniformBuffer, descriptorPoolCreateInfo.maxSets}
        );

        Chicane::Renderer::VulkanDescriptorPool::init(
            m_frameDescriptor.pool,
            backend->logicalDevice,
            descriptorPoolCreateInfo
        );

        for (Chicane::Renderer::VulkanFrame& frame : backend->frames)
        {
            vk::DescriptorSet descriptorSet;

            Chicane::Renderer::VulkanDescriptorSetLayout::allocate(
                descriptorSet,
                backend->logicalDevice,
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

    void VulkanLUI::destroyFrameResources()
    {
        Chicane::Renderer::VulkanBackend* backend = getBackend<Chicane::Renderer::VulkanBackend>();

        backend->logicalDevice.destroyDescriptorSetLayout(m_frameDescriptor.setLayout);
        backend->logicalDevice.destroyDescriptorPool(m_frameDescriptor.pool);
    }

    void VulkanLUI::initGraphicsPipeline()
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
        colorAttachment.initialLayout = vk::ImageLayout::ePresentSrcKHR;
        colorAttachment.finalLayout   = vk::ImageLayout::ePresentSrcKHR;

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
            .addViewport(backend->getVkViewport(this))
            .addDynamicState(vk::DynamicState::eViewport)
            .addScissor(backend->getVkScissor(this))
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

    void VulkanLUI::initFramebuffers()
    {
        Chicane::Renderer::VulkanBackend* backend  = getBackend<Chicane::Renderer::VulkanBackend>();
        vk::Viewport                      viewport = backend->getVkViewport(this);

        for (Chicane::Renderer::VulkanSwapchainImage& image : backend->swapchain.images)
        {
            Chicane::Renderer::VulkanFrameBufferCreateInfo createInfo;
            createInfo.id            = m_id;
            createInfo.logicalDevice = backend->logicalDevice;
            createInfo.renderPass    = m_graphicsPipeline.renderPass;
            createInfo.extent.width  = viewport.width;
            createInfo.extent.height = viewport.height;
            createInfo.attachments.push_back(image.colorImage.view);
            createInfo.attachments.push_back(image.depthImage.view);

            image.addBuffer(createInfo);
        }
    }

    void VulkanLUI::initViewport()
    {
        Chicane::Renderer::ViewportSettings viewport;
        viewport.width  = "85vw";
        viewport.height = "80vh";

        setViewport(viewport);

        for (Layer* layer :
             m_backend->findLayers([](const Layer* inLayer)
                                   { return inLayer->getId().contains(Chicane::Renderer::SCENE_LAYER_ID); }))
        {
            layer->setViewport(viewport);
        }
    }
}