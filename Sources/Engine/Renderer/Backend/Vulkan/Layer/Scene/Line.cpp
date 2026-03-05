#include "Chicane/Renderer/Backend/Vulkan/Layer/Scene/Line.hpp"

#include "Chicane/Renderer/Backend/Vulkan.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout/BidingsCreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Frame.hpp"
#include "Chicane/Renderer/Backend/Vulkan/GraphicsPipeline/Builder.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Layer/Scene.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Vertex.hpp"

namespace Chicane
{
    namespace Renderer
    {
        VulkanLSceneLine::VulkanLSceneLine()
            : Layer(SCENE_LINE_LAYER_ID),
              m_clear({vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f), vk::ClearDepthStencilValue(1.0f, 0)})
        {}

        void VulkanLSceneLine::onInit()
        {
            initFrameResources();

            initGraphicsPipeline();
            initFramebuffers();
        }

        void VulkanLSceneLine::onRestart()
        {
            initFramebuffers();
        }

        void VulkanLSceneLine::onDestruction()
        {
            destroyFrameResources();

            m_graphicsPipeline.destroy();
        }

        bool VulkanLSceneLine::onBeginRender(const Frame& inFrame)
        {
            if (!inFrame.hasDraws(DrawPolyType::e3D, DrawPolyMode::Line))
            {
                return false;
            }

            return true;
        }

        void VulkanLSceneLine::onRender(const Frame& inFrame, void* inData)
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();
            VulkanLScene*  parent  = backend->getLayer<VulkanLScene>(SCENE_LAYER_ID);

            VulkanFrame       frame         = *((VulkanFrame*)inData);
            vk::CommandBuffer commandBuffer = frame.commandBuffer;

            vk::Viewport viewport = backend->getVkViewport(this);
            commandBuffer.setViewport(0, 1, &viewport);

            vk::Rect2D scissor = backend->getVkScissor(this);
            commandBuffer.setScissor(0, 1, &scissor);

            vk::RenderPassBeginInfo beginInfo;
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
            vk::Buffer     vertexBuffers[] = {parent->modelVertexBuffer.instance};
            vk::DeviceSize offsets[]       = {0};

            commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);

            commandBuffer.bindIndexBuffer(parent->modelIndexBuffer.instance, 0, vk::IndexType::eUint32);

            for (const DrawPoly& draw : inFrame.getDraws(DrawPolyType::e3D, DrawPolyMode::Line))
            {
                commandBuffer.drawIndexed(
                    draw.indexCount,
                    draw.instanceCount,
                    draw.indexStart,
                    draw.vertexStart,
                    draw.instanceStart
                );
            }
            commandBuffer.endRenderPass();
        }

        void VulkanLSceneLine::initFrameResources()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            VulkanDescriptorSetLayoutBidingsCreateInfo bidings;
            bidings.count = 1;

            // Camera
            bidings.indices.push_back(0);
            bidings.types.push_back(vk::DescriptorType::eUniformBuffer);
            bidings.counts.push_back(1);
            bidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

            VulkanDescriptorSetLayout::init(m_frameDescriptor.setLayout, backend->logicalDevice, bidings);

            VulkanDescriptorPoolCreateInfo descriptorPoolCreateInfo;
            descriptorPoolCreateInfo.maxSets = static_cast<std::uint32_t>(backend->frames.size());
            descriptorPoolCreateInfo.sizes.push_back(
                {vk::DescriptorType::eUniformBuffer, descriptorPoolCreateInfo.maxSets}
            );

            VulkanDescriptorPool::init(m_frameDescriptor.pool, backend->logicalDevice, descriptorPoolCreateInfo);

            for (VulkanFrame& frame : backend->frames)
            {
                vk::DescriptorSet descriptorSet;

                VulkanDescriptorSetLayout::allocate(
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

        void VulkanLSceneLine::destroyFrameResources()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            backend->logicalDevice.destroyDescriptorSetLayout(m_frameDescriptor.setLayout);
            backend->logicalDevice.destroyDescriptorPool(m_frameDescriptor.pool);
        }

        void VulkanLSceneLine::initGraphicsPipeline()
        {
            // Backend
            VulkanBackend* backend = getBackend<VulkanBackend>();

            // Shader
            VulkanShaderStageCreateInfo vertexShader;
            vertexShader.path = "Contents/Engine/Shaders/Vulkan/Scene/Line.vvert";
            vertexShader.type = vk::ShaderStageFlagBits::eVertex;

            VulkanShaderStageCreateInfo fragmentShader;
            fragmentShader.path = "Contents/Engine/Shaders/Vulkan/Scene/Line.vfrag";
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
            rasterization.polygonMode             = vk::PolygonMode::eLine;
            rasterization.cullMode                = vk::CullModeFlagBits::eNone;
            rasterization.frontFace               = vk::FrontFace::eCounterClockwise;

            // Build
            VulkanGraphicsPipelineBuilder()
                .addVertexBinding(VulkanVertex::getBindingDescription())
                .addVertexAttributes(VulkanVertex::getAttributeDescriptions())
                .setInputAssembly(VulkanGraphicsPipeline::createInputAssemblyState())
                .addViewport(backend->getVkViewport(this))
                .addDynamicState(vk::DynamicState::eViewport)
                .addScissor(backend->getVkScissor(this))
                .addDynamicState(vk::DynamicState::eScissor)
                .addShaderStage(vertexShader, backend->logicalDevice)
                .addShaderStage(fragmentShader, backend->logicalDevice)
                .addColorBlendingAttachment(VulkanGraphicsPipeline::createBlendAttachmentState(false))
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

        void VulkanLSceneLine::initFramebuffers()
        {
            VulkanBackend* backend  = getBackend<VulkanBackend>();
            vk::Viewport   viewport = backend->getVkViewport(this);

            for (VulkanSwapchainImage& frame : backend->swapchain.images)
            {
                VulkanFrameBufferCreateInfo createInfo;
                createInfo.id            = m_id;
                createInfo.logicalDevice = backend->logicalDevice;
                createInfo.renderPass    = m_graphicsPipeline.renderPass;
                createInfo.extent.width  = viewport.width;
                createInfo.extent.height = viewport.height;
                createInfo.attachments.push_back(frame.colorImage.view);
                createInfo.attachments.push_back(frame.depthImage.view);

                frame.addBuffer(createInfo);
            }
        }
    }
}