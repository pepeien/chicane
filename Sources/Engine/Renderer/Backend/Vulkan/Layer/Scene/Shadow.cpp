#include "Chicane/Renderer/Backend/Vulkan/Layer/Scene/Shadow.hpp"

#include "Chicane/Renderer/Backend/Vulkan.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout/BidingsCreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/GraphicsPipeline/Builder.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Layer/Scene.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Vertex.hpp"

namespace Chicane
{
    namespace Renderer
    {
        VulkanLSceneShadow::VulkanLSceneShadow()
            : Layer("Engine_Scene_Shadow"),
              m_clear({vk::ClearDepthStencilValue(1.0f, 0)})
        {}

        bool VulkanLSceneShadow::onInit()
        {
            initFrameResources();

            initGraphicsPipeline();
            initFramebuffers();

            return true;
        }

        bool VulkanLSceneShadow::onDestroy()
        {
            destroyFrameResources();

            return true;
        }

        bool VulkanLSceneShadow::onRebuild()
        {
            initFrameResources();
            initFramebuffers();

            return true;
        }

        bool VulkanLSceneShadow::onSetup(const Frame& inFrame)
        {
            if (inFrame.getInstances3D().empty() || inFrame.get3DDraws().empty())
            {
                return false;
            }

            return true;
        }

        void VulkanLSceneShadow::onRender(const Frame& inFrame, void* inData)
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();
            VulkanLScene*  parent  = backend->getLayer<VulkanLScene>();

            VulkanSwapchainImage image         = *((VulkanSwapchainImage*)inData);
            vk::CommandBuffer    commandBuffer = image.commandBuffer;

            vk::RenderPassBeginInfo beginInfo = {};
            beginInfo.renderPass              = m_graphicsPipeline.renderPass;
            beginInfo.framebuffer             = image.getFramebuffer(m_id);
            beginInfo.renderArea.extent       = backend->swapchain.extent;
            beginInfo.clearValueCount         = static_cast<std::uint32_t>(m_clear.size());
            beginInfo.pClearValues            = m_clear.data();

            commandBuffer.beginRenderPass(&beginInfo, vk::SubpassContents::eInline);
            // Pipeline
            m_graphicsPipeline.bind(commandBuffer);

            // Frame
            m_graphicsPipeline.bindDescriptorSet(commandBuffer, 0, image.getDescriptorSet(m_id));

            // Draw
            vk::Buffer     vertexBuffers[] = {parent->modelVertexBuffer.instance};
            vk::DeviceSize offsets[]       = {0};

            commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);

            commandBuffer.bindIndexBuffer(parent->modelIndexBuffer.instance, 0, vk::IndexType::eUint32);

            for (const DrawPoly& draw : inFrame.get3DDraws())
            {
                if (draw.instanceCount == 0)
                {
                    continue;
                }

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

        void VulkanLSceneShadow::initFrameResources()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            VulkanDescriptorSetLayoutBidingsCreateInfo bidings;
            bidings.count = 2;

            // Light
            bidings.indices.push_back(0);
            bidings.types.push_back(vk::DescriptorType::eUniformBuffer);
            bidings.counts.push_back(1);
            bidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

            // Poly 3D
            bidings.indices.push_back(1);
            bidings.types.push_back(vk::DescriptorType::eStorageBuffer);
            bidings.counts.push_back(1);
            bidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

            VulkanDescriptorSetLayout::init(m_frameDescriptor.setLayout, backend->logicalDevice, bidings);

            VulkanDescriptorPoolCreateInfo descriptorPoolCreateInfo;
            descriptorPoolCreateInfo.maxSets = static_cast<std::uint32_t>(backend->swapchain.images.size());
            descriptorPoolCreateInfo.sizes.push_back(
                {vk::DescriptorType::eUniformBuffer, descriptorPoolCreateInfo.maxSets}
            );
            descriptorPoolCreateInfo.sizes.push_back(
                {vk::DescriptorType::eStorageBuffer, descriptorPoolCreateInfo.maxSets}
            );

            VulkanDescriptorPool::init(m_frameDescriptor.pool, backend->logicalDevice, descriptorPoolCreateInfo);

            for (VulkanSwapchainImage& image : backend->swapchain.images)
            {
                vk::DescriptorSet descriptorSet;

                VulkanDescriptorSetLayout::allocate(
                    descriptorSet,
                    backend->logicalDevice,
                    m_frameDescriptor.setLayout,
                    m_frameDescriptor.pool
                );
                image.addDescriptorSet(m_id, descriptorSet);

                vk::WriteDescriptorSet lightWriteInfo;
                lightWriteInfo.dstSet          = descriptorSet;
                lightWriteInfo.dstBinding      = 0;
                lightWriteInfo.dstArrayElement = 0;
                lightWriteInfo.descriptorCount = 1;
                lightWriteInfo.descriptorType  = vk::DescriptorType::eUniformBuffer;
                lightWriteInfo.pBufferInfo     = &image.lightResource.bufferInfo;
                image.addWriteDescriptorSet(lightWriteInfo);

                vk::WriteDescriptorSet poly3DWriteInfo;
                poly3DWriteInfo.dstSet          = descriptorSet;
                poly3DWriteInfo.dstBinding      = 1;
                poly3DWriteInfo.dstArrayElement = 0;
                poly3DWriteInfo.descriptorCount = 1;
                poly3DWriteInfo.descriptorType  = vk::DescriptorType::eStorageBuffer;
                poly3DWriteInfo.pBufferInfo     = &image.poly3DResource.bufferInfo;
                image.addWriteDescriptorSet(poly3DWriteInfo);
            }
        }

        void VulkanLSceneShadow::destroyFrameResources()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            backend->logicalDevice.destroyDescriptorSetLayout(m_frameDescriptor.setLayout);
            backend->logicalDevice.destroyDescriptorPool(m_frameDescriptor.pool);
        }

        void VulkanLSceneShadow::initGraphicsPipeline()
        {
            // Backend
            VulkanBackend* backend = getBackend<VulkanBackend>();
            VulkanLScene*  parent  = backend->getLayer<VulkanLScene>();

            // Shader
            VulkanShaderStageCreateInfo vertexShader;
            vertexShader.path = "Contents/Engine/Shaders/Vulkan/Scene/Shadow.vvert";
            vertexShader.type = vk::ShaderStageFlagBits::eVertex;

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

            // Attachments
            vk::AttachmentDescription depthAttachment;
            depthAttachment.flags         = vk::AttachmentDescriptionFlags();
            depthAttachment.format        = backend->swapchain.depthFormat;
            depthAttachment.samples       = vk::SampleCountFlagBits::e1;
            depthAttachment.loadOp        = vk::AttachmentLoadOp::eClear;
            depthAttachment.storeOp       = vk::AttachmentStoreOp::eStore;
            depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
            depthAttachment.finalLayout   = vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal;

            vk::AttachmentReference depthReference;
            depthReference.attachment = 0;
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
            subpass.pDepthStencilAttachment = &depthReference;

            // Rasterizer
            vk::PipelineRasterizationStateCreateInfo rasterization;
            rasterization.flags                   = vk::PipelineRasterizationStateCreateFlags();
            rasterization.depthClampEnable        = VK_FALSE;
            rasterization.rasterizerDiscardEnable = VK_FALSE;
            rasterization.depthBiasEnable         = VK_TRUE;
            rasterization.depthBiasClamp          = 0.0f;
            rasterization.polygonMode             = vk::PolygonMode::eFill;
            rasterization.cullMode                = vk::CullModeFlagBits::eFront;
            rasterization.frontFace               = vk::FrontFace::eClockwise;
            rasterization.lineWidth               = 1.0f;
            rasterization.depthBiasConstantFactor = 1.25f;
            rasterization.depthBiasSlopeFactor    = 1.75f;

            // Build
            VulkanGraphicsPipelineBuilder()
                .addVertexBinding(VulkanVertex::getBindingDescription())
                .addVertexAttributes(VulkanVertex::getAttributeDescriptions())
                .setInputAssembly(VulkanGraphicsPipeline::createInputAssemblyState())
                .addViewport(backend->viewport)
                .addDynamicState(vk::DynamicState::eViewport)
                .addScissor(backend->scissor)
                .addDynamicState(vk::DynamicState::eScissor)
                .addShaderStage(vertexShader, backend->logicalDevice)
                .setDepthStencil(depth)
                .addAttachment(depthAttachment)
                .addSubpassDependecy(depthSubpassDepedency)
                .addSubpass(subpass)
                .addDescriptorSetLayout(m_frameDescriptor.setLayout)
                .setRasterization(rasterization)
                .build(m_graphicsPipeline, backend->logicalDevice);
        }

        void VulkanLSceneShadow::initFramebuffers()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            for (VulkanSwapchainImage& image : backend->swapchain.images)
            {
                VulkanFrameCreateInfo createInfo;
                createInfo.id            = m_id;
                createInfo.logicalDevice = backend->logicalDevice;
                createInfo.renderPass    = m_graphicsPipeline.renderPass;
                createInfo.extent        = backend->swapchain.extent;
                createInfo.attachments.push_back(image.shadowImage.view);

                image.addBuffer(createInfo);
            }
        }
    }
}