#include "Chicane/Renderer/Backend/Vulkan/Layer/Scene/Shadow.hpp"

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
        VulkanLSceneShadow::VulkanLSceneShadow()
            : Layer(SCENE_SHADOW_LAYER_ID),
              m_clear({vk::ClearDepthStencilValue(1.0f, 0)})
        {}

        void VulkanLSceneShadow::onInit()
        {
            initFrameResources();

            initGraphicsPipeline();
            initFramebuffers();
        }

        void VulkanLSceneShadow::onRestart()
        {
            initFramebuffers();
        }

        void VulkanLSceneShadow::onDestruction()
        {
            destroyFrameResources();

            m_graphicsPipeline.destroy();
        }

        void VulkanLSceneShadow::onRender(const Frame& inFrame, void* inData)
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();
            VulkanLScene*  parent  = backend->getLayer<VulkanLScene>(SCENE_LAYER_ID);

            VulkanFrame&      frame         = *((VulkanFrame*)inData);
            vk::CommandBuffer commandBuffer = frame.commandBuffer;

            vk::Viewport viewport;
            viewport.x        = 0.0f;
            viewport.y        = 0.0f;
            viewport.width    = static_cast<float>(SHADOW_MAP_WIDTH);
            viewport.height   = static_cast<float>(SHADOW_MAP_HEIGHT);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;

            vk::Rect2D scissor;
            scissor.offset.x      = 0;
            scissor.offset.y      = 0;
            scissor.extent.width  = SHADOW_MAP_WIDTH;
            scissor.extent.height = SHADOW_MAP_HEIGHT;

            for (std::uint32_t cascade = 0; cascade < SHADOW_CASCADE_COUNT; ++cascade)
            {
                vk::RenderPassBeginInfo beginInfo;
                beginInfo.renderPass = m_graphicsPipeline.renderPass;
                beginInfo.framebuffer =
                    frame.image.getFramebuffer(String(SCENE_SHADOW_LAYER_ID) + "_" + std::to_string(cascade));
                beginInfo.renderArea.extent.width  = SHADOW_MAP_WIDTH;
                beginInfo.renderArea.extent.height = SHADOW_MAP_HEIGHT;
                beginInfo.clearValueCount          = static_cast<std::uint32_t>(m_clear.size());
                beginInfo.pClearValues             = m_clear.data();

                commandBuffer.beginRenderPass(&beginInfo, vk::SubpassContents::eInline);
                commandBuffer.setViewport(0, 1, &viewport);
                commandBuffer.setScissor(0, 1, &scissor);
                commandBuffer.setLineWidth(1.0f);

                m_graphicsPipeline.bind(commandBuffer);
                m_graphicsPipeline.bind(commandBuffer, 0, frame.getDescriptorSet(m_id));
                commandBuffer.pushConstants(
                    m_graphicsPipeline.layout,
                    vk::ShaderStageFlagBits::eVertex,
                    0,
                    sizeof(std::uint32_t),
                    &cascade
                );

                vk::Buffer     vertexBuffers[] = {parent->modelVertexBuffer.instance};
                vk::DeviceSize offsets[]       = {0};
                commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
                commandBuffer.bindIndexBuffer(parent->modelIndexBuffer.instance, 0, vk::IndexType::eUint32);

                for (const DrawPoly& draw : inFrame.getShadowDraws())
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
            descriptorPoolCreateInfo.maxSets = static_cast<std::uint32_t>(backend->frames.size());
            descriptorPoolCreateInfo.sizes.push_back(
                {vk::DescriptorType::eUniformBuffer, descriptorPoolCreateInfo.maxSets}
            );
            descriptorPoolCreateInfo.sizes.push_back(
                {vk::DescriptorType::eStorageBuffer, descriptorPoolCreateInfo.maxSets}
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

                vk::WriteDescriptorSet lightWriteInfo;
                lightWriteInfo.dstSet          = descriptorSet;
                lightWriteInfo.dstBinding      = 0;
                lightWriteInfo.dstArrayElement = 0;
                lightWriteInfo.descriptorCount = 1;
                lightWriteInfo.descriptorType  = vk::DescriptorType::eUniformBuffer;
                lightWriteInfo.pBufferInfo     = &frame.lightResource.bufferInfo;
                frame.addWriteDescriptorSet(lightWriteInfo);

                vk::WriteDescriptorSet poly3DWriteInfo;
                poly3DWriteInfo.dstSet          = descriptorSet;
                poly3DWriteInfo.dstBinding      = 1;
                poly3DWriteInfo.dstArrayElement = 0;
                poly3DWriteInfo.descriptorCount = 1;
                poly3DWriteInfo.descriptorType  = vk::DescriptorType::eStorageBuffer;
                poly3DWriteInfo.pBufferInfo     = &frame.poly3DResource.bufferInfo;
                frame.addWriteDescriptorSet(poly3DWriteInfo);
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
            VulkanLScene*  parent  = backend->getLayer<VulkanLScene>(SCENE_LAYER_ID);

            // Shader
            VulkanShaderStageCreateInfo vertexShader;
            vertexShader.path = "Assets/Engine/Shaders/Vulkan/Scene/Shadow.vvert";
            vertexShader.type = vk::ShaderStageFlagBits::eVertex;

            // Depth
            vk::PipelineDepthStencilStateCreateInfo depth;
            depth.depthBoundsTestEnable = false;
            depth.stencilTestEnable     = false;
            depth.depthWriteEnable      = true;
            depth.depthTestEnable       = true;
            depth.depthCompareOp        = vk::CompareOp::eLess;
            depth.minDepthBounds        = 0.0f;
            depth.maxDepthBounds        = 1.0f;

            // Attachments
            vk::AttachmentDescription depthAttachment;
            depthAttachment.format        = parent->shadowImage.format;
            depthAttachment.samples       = vk::SampleCountFlagBits::e1;
            depthAttachment.loadOp        = vk::AttachmentLoadOp::eClear;
            depthAttachment.storeOp       = vk::AttachmentStoreOp::eStore;
            depthAttachment.initialLayout = vk::ImageLayout::eDepthStencilReadOnlyOptimal;
            depthAttachment.finalLayout   = vk::ImageLayout::eDepthStencilReadOnlyOptimal;

            vk::AttachmentReference depthReference;
            depthReference.attachment = 0;
            depthReference.layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal;

            vk::SubpassDependency depthSubpassBegin;
            depthSubpassBegin.srcSubpass    = VK_SUBPASS_EXTERNAL;
            depthSubpassBegin.dstSubpass    = 0;
            depthSubpassBegin.srcStageMask  = vk::PipelineStageFlagBits::eFragmentShader;
            depthSubpassBegin.dstStageMask  = vk::PipelineStageFlagBits::eEarlyFragmentTests;
            depthSubpassBegin.srcAccessMask = vk::AccessFlagBits::eShaderRead;
            depthSubpassBegin.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;

            vk::SubpassDependency depthSubpassEnd;
            depthSubpassEnd.srcSubpass    = 0;
            depthSubpassEnd.dstSubpass    = VK_SUBPASS_EXTERNAL;
            depthSubpassEnd.srcStageMask  = vk::PipelineStageFlagBits::eLateFragmentTests;
            depthSubpassEnd.dstStageMask  = vk::PipelineStageFlagBits::eFragmentShader;
            depthSubpassEnd.srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
            depthSubpassEnd.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            vk::SubpassDescription subpass;
            subpass.pipelineBindPoint       = vk::PipelineBindPoint::eGraphics;
            subpass.pDepthStencilAttachment = &depthReference;

            // Rasterizer
            vk::PipelineRasterizationStateCreateInfo rasterization;
            rasterization.rasterizerDiscardEnable = false;
            rasterization.depthClampEnable        = false;
            rasterization.depthBiasEnable         = true;
            rasterization.depthBiasConstantFactor = 1.25f;
            rasterization.depthBiasSlopeFactor    = 1.75f;
            rasterization.polygonMode             = vk::PolygonMode::eFill;
            rasterization.cullMode                = vk::CullModeFlagBits::eFront;
            rasterization.frontFace               = vk::FrontFace::eCounterClockwise;

            // Build
            vk::PushConstantRange cascadePush;
            cascadePush.stageFlags = vk::ShaderStageFlagBits::eVertex;
            cascadePush.offset     = 0;
            cascadePush.size       = sizeof(std::uint32_t);

            VulkanGraphicsPipelineBuilder()
                .addVertexBinding(VulkanVertex::getBindingDescription())
                .addVertexAttributes(VulkanVertex::getAttributeDescriptions())
                .setInputAssembly(VulkanGraphicsPipeline::createInputAssemblyState())
                .addViewport(backend->getVkViewport(this))
                .addDynamicState(vk::DynamicState::eViewport)
                .addScissor(backend->getVkScissor(this))
                .addDynamicState(vk::DynamicState::eScissor)
                .addDynamicState(vk::DynamicState::eLineWidth)
                .addShaderStage(vertexShader, backend->logicalDevice)
                .setDepthStencil(depth)
                .addAttachment(depthAttachment)
                .addSubpassDependecy(depthSubpassBegin)
                .addSubpassDependecy(depthSubpassEnd)
                .addSubpass(subpass)
                .addDescriptorSetLayout(m_frameDescriptor.setLayout)
                .addPushConstant(cascadePush)
                .setRasterization(rasterization)
                .build(m_graphicsPipeline, backend->logicalDevice);
        }

        void VulkanLSceneShadow::initFramebuffers()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();
            VulkanLScene*  parent  = backend->getLayer<VulkanLScene>(SCENE_LAYER_ID);

            for (VulkanSwapchainImage& image : backend->swapchain.images)
            {
                for (std::uint32_t cascade = 0; cascade < SHADOW_CASCADE_COUNT; ++cascade)
                {
                    VulkanFrameBufferCreateInfo createInfo;
                    createInfo.id            = String(SCENE_SHADOW_LAYER_ID) + "_" + std::to_string(cascade);
                    createInfo.logicalDevice = backend->logicalDevice;
                    createInfo.renderPass    = m_graphicsPipeline.renderPass;
                    createInfo.extent.width  = SHADOW_MAP_WIDTH;
                    createInfo.extent.height = SHADOW_MAP_HEIGHT;
                    createInfo.attachments.push_back(parent->shadowLayerViews[cascade]);

                    image.addBuffer(createInfo);
                }
            }
        }
    }
}