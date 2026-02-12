#include "Chicane/Renderer/Backend/Vulkan/Layer/Scene/Mesh.hpp"

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
        VulkanLSceneMesh::VulkanLSceneMesh()
            : Layer("Engine_Scene_Mesh"),
              m_clear({vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f), vk::ClearDepthStencilValue(1.0f, 0)})
        {}

        bool VulkanLSceneMesh::onInit()
        {
            initFrameResources();

            initGraphicsPipeline();
            initFramebuffers();

            return true;
        }

        bool VulkanLSceneMesh::onDestroy()
        {
            destroyFrameResources();

            return true;
        }

        bool VulkanLSceneMesh::onRebuild()
        {
            initFrameResources();
            initFramebuffers();

            return true;
        }

        bool VulkanLSceneMesh::onSetup(const Frame& inFrame)
        {
            if (inFrame.getInstances3D().empty() || inFrame.get3DDraws().empty())
            {
                return false;
            }

            return true;
        }

        void VulkanLSceneMesh::onRender(const Frame& inFrame, void* inData)
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

            // Texture
            m_graphicsPipeline.bindDescriptorSet(commandBuffer, 1, backend->textureDescriptor.set);

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

        void VulkanLSceneMesh::initFrameResources()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            VulkanDescriptorSetLayoutBidingsCreateInfo bidings;
            bidings.count = 4;

            // Camera
            bidings.indices.push_back(0);
            bidings.types.push_back(vk::DescriptorType::eUniformBuffer);
            bidings.counts.push_back(1);
            bidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

            // Light
            bidings.indices.push_back(1);
            bidings.types.push_back(vk::DescriptorType::eUniformBuffer);
            bidings.counts.push_back(1);
            bidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

            // Poly 3D
            bidings.indices.push_back(2);
            bidings.types.push_back(vk::DescriptorType::eStorageBuffer);
            bidings.counts.push_back(1);
            bidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

            // Shadow
            bidings.indices.push_back(3);
            bidings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
            bidings.counts.push_back(1);
            bidings.stages.push_back(vk::ShaderStageFlagBits::eFragment);

            VulkanDescriptorSetLayout::init(m_frameDescriptor.setLayout, backend->logicalDevice, bidings);

            VulkanDescriptorPoolCreateInfo descriptorPoolCreateInfo;
            descriptorPoolCreateInfo.maxSets = static_cast<std::uint32_t>(backend->swapchain.images.size());
            descriptorPoolCreateInfo.sizes.push_back(
                {vk::DescriptorType::eUniformBuffer, descriptorPoolCreateInfo.maxSets}
            );
            descriptorPoolCreateInfo.sizes.push_back(
                {vk::DescriptorType::eUniformBuffer, descriptorPoolCreateInfo.maxSets}
            );
            descriptorPoolCreateInfo.sizes.push_back(
                {vk::DescriptorType::eStorageBuffer, descriptorPoolCreateInfo.maxSets}
            );
            descriptorPoolCreateInfo.sizes.push_back(
                {vk::DescriptorType::eCombinedImageSampler, descriptorPoolCreateInfo.maxSets}
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

                vk::WriteDescriptorSet cameraWriteInfo;
                cameraWriteInfo.dstSet          = descriptorSet;
                cameraWriteInfo.dstBinding      = 0;
                cameraWriteInfo.dstArrayElement = 0;
                cameraWriteInfo.descriptorCount = 1;
                cameraWriteInfo.descriptorType  = vk::DescriptorType::eUniformBuffer;
                cameraWriteInfo.pBufferInfo     = &image.cameraResource.bufferInfo;
                image.addWriteDescriptorSet(cameraWriteInfo);

                vk::WriteDescriptorSet lightWriteInfo;
                lightWriteInfo.dstSet          = descriptorSet;
                lightWriteInfo.dstBinding      = 1;
                lightWriteInfo.dstArrayElement = 0;
                lightWriteInfo.descriptorCount = 1;
                lightWriteInfo.descriptorType  = vk::DescriptorType::eUniformBuffer;
                lightWriteInfo.pBufferInfo     = &image.lightResource.bufferInfo;
                image.addWriteDescriptorSet(lightWriteInfo);

                vk::WriteDescriptorSet poly3DWriteInfo;
                poly3DWriteInfo.dstSet          = descriptorSet;
                poly3DWriteInfo.dstBinding      = 2;
                poly3DWriteInfo.dstArrayElement = 0;
                poly3DWriteInfo.descriptorCount = 1;
                poly3DWriteInfo.descriptorType  = vk::DescriptorType::eStorageBuffer;
                poly3DWriteInfo.pBufferInfo     = &image.poly3DResource.bufferInfo;
                image.addWriteDescriptorSet(poly3DWriteInfo);

                vk::WriteDescriptorSet shadowWriteInfo;
                shadowWriteInfo.dstSet          = descriptorSet;
                shadowWriteInfo.dstBinding      = 3;
                shadowWriteInfo.dstArrayElement = 0;
                shadowWriteInfo.descriptorCount = 1;
                shadowWriteInfo.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
                shadowWriteInfo.pImageInfo      = &image.shadowImageInfo;
                image.addWriteDescriptorSet(shadowWriteInfo);
            }
        }

        void VulkanLSceneMesh::destroyFrameResources()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            backend->logicalDevice.destroyDescriptorSetLayout(m_frameDescriptor.setLayout);
            backend->logicalDevice.destroyDescriptorPool(m_frameDescriptor.pool);
        }

        void VulkanLSceneMesh::initGraphicsPipeline()
        {
            // Backend
            VulkanBackend* backend = getBackend<VulkanBackend>();

            // Shader
            VulkanShaderStageCreateInfo vertexShader;
            vertexShader.path = "Contents/Engine/Shaders/Vulkan/Scene/Mesh.vvert";
            vertexShader.type = vk::ShaderStageFlagBits::eVertex;

            VulkanShaderStageCreateInfo fragmentShader;
            fragmentShader.path = "Contents/Engine/Shaders/Vulkan/Scene/Mesh.vfrag";
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
            depthAttachment.loadOp        = vk::AttachmentLoadOp::eClear;
            depthAttachment.storeOp       = vk::AttachmentStoreOp::eStore;
            depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
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
            VulkanGraphicsPipelineBuilder()
                .addVertexBinding(VulkanVertex::getBindingDescription())
                .addVertexAttributes(VulkanVertex::getAttributeDescriptions())
                .setInputAssembly(VulkanGraphicsPipeline::createInputAssemblyState())
                .addViewport(backend->viewport)
                .addDynamicState(vk::DynamicState::eViewport)
                .addScissor(backend->scissor)
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
                .addDescriptorSetLayout(backend->textureDescriptor.setLayout)
                .setRasterization(rasterization)
                .build(m_graphicsPipeline, backend->logicalDevice);
        }

        void VulkanLSceneMesh::initFramebuffers()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            for (VulkanSwapchainImage& frame : backend->swapchain.images)
            {
                VulkanFrameCreateInfo createInfo;
                createInfo.id            = m_id;
                createInfo.logicalDevice = backend->logicalDevice;
                createInfo.renderPass    = m_graphicsPipeline.renderPass;
                createInfo.extent        = backend->swapchain.extent;
                createInfo.attachments.push_back(frame.colorImage.view);
                createInfo.attachments.push_back(frame.depthImage.view);

                frame.addBuffer(createInfo);
            }
        }
    }
}