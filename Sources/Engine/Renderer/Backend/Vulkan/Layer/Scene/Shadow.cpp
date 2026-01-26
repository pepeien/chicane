#include "Chicane/Renderer/Backend/Vulkan/Layer/Scene/Shadow.hpp"

#include "Chicane/Renderer/Backend/Vulkan.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Data.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout/BidingsCreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Layer/Scene.hpp"

namespace Chicane
{
    namespace Renderer
    {
        VulkanLSceneShadow::VulkanLSceneShadow()
            : Layer("Engine_Scene_Shadow"),
              m_clear({vk::ClearDepthStencilValue(1.0f, 0)})
        {}

        VulkanLSceneShadow::~VulkanLSceneShadow()
        {
            deleteChildren();
            destroyFrameResources();

            m_graphicsPipeline.reset();
        }

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

        void VulkanLSceneShadow::onRender(const Frame& inFrame, void* inData)
        {
            VulkanBackendData* data          = (VulkanBackendData*)inData;
            vk::CommandBuffer& commandBuffer = data->commandBuffer;
            VulkanFrame&       frame         = data->frame;

            vk::RenderPassBeginInfo beginInfo = {};
            beginInfo.renderPass              = m_graphicsPipeline->renderPass;
            beginInfo.framebuffer             = frame.getFramebuffer(m_id);
            beginInfo.renderArea.extent       = getBackend<VulkanBackend>()->swapchain.extent;
            beginInfo.clearValueCount         = static_cast<std::uint32_t>(m_clear.size());
            beginInfo.pClearValues            = m_clear.data();

            commandBuffer.beginRenderPass(&beginInfo, vk::SubpassContents::eInline);
            // Pipeline
            m_graphicsPipeline->bind(commandBuffer);

            // Frame
            m_graphicsPipeline->bindDescriptorSet(commandBuffer, 0, frame.getDescriptorSet(m_id));

            // Draw
            vk::Buffer     vertexBuffers[] = {getParent<VulkanLScene>()->modelVertexBuffer.instance};
            vk::DeviceSize offsets[]       = {0};

            commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);

            commandBuffer
                .bindIndexBuffer(getParent<VulkanLScene>()->modelIndexBuffer.instance, 0, vk::IndexType::eUint32);

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

            VulkanDescriptorSetLayout::init(
                m_frameDescriptor.setLayout,
                getBackend<VulkanBackend>()->logicalDevice,
                bidings
            );

            VulkanDescriptorPoolCreateInfo descriptorPoolCreateInfo;
            descriptorPoolCreateInfo.maxSets =
                static_cast<std::uint32_t>(getBackend<VulkanBackend>()->swapchain.frames.size());
            descriptorPoolCreateInfo.sizes.push_back(
                {vk::DescriptorType::eUniformBuffer, descriptorPoolCreateInfo.maxSets}
            );
            descriptorPoolCreateInfo.sizes.push_back(
                {vk::DescriptorType::eStorageBuffer, descriptorPoolCreateInfo.maxSets}
            );

            VulkanDescriptorPool::init(
                m_frameDescriptor.pool,
                getBackend<VulkanBackend>()->logicalDevice,
                descriptorPoolCreateInfo
            );

            for (VulkanFrame& frame : getBackend<VulkanBackend>()->swapchain.frames)
            {
                vk::DescriptorSet descriptorSet;

                VulkanDescriptorSetLayout::allocate(
                    descriptorSet,
                    getBackend<VulkanBackend>()->logicalDevice,
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
            getBackend<VulkanBackend>()->logicalDevice.destroyDescriptorSetLayout(m_frameDescriptor.setLayout);
            getBackend<VulkanBackend>()->logicalDevice.destroyDescriptorPool(m_frameDescriptor.pool);
        }

        void VulkanLSceneShadow::initGraphicsPipeline()
        {
            // Rasterizer
            vk::PipelineRasterizationStateCreateInfo rasterizeCreateInfo = {};
            rasterizeCreateInfo.flags                                    = vk::PipelineRasterizationStateCreateFlags();
            rasterizeCreateInfo.depthClampEnable                         = VK_FALSE;
            rasterizeCreateInfo.rasterizerDiscardEnable                  = VK_FALSE;
            rasterizeCreateInfo.depthBiasEnable                          = VK_TRUE;
            rasterizeCreateInfo.depthBiasClamp                           = 0.0f;
            rasterizeCreateInfo.polygonMode                              = vk::PolygonMode::eFill;
            rasterizeCreateInfo.cullMode                                 = vk::CullModeFlagBits::eBack;
            rasterizeCreateInfo.frontFace                                = vk::FrontFace::eClockwise;
            rasterizeCreateInfo.lineWidth                                = 1.0f;
            rasterizeCreateInfo.depthBiasConstantFactor                  = 1.25f;
            rasterizeCreateInfo.depthBiasSlopeFactor                     = 1.75f;

            // Shader
            VulkanShaderStageCreateInfo vertexShader;
            vertexShader.path = "Contents/Engine/Shaders/Vulkan/Scene/Shadow.vvert";
            vertexShader.type = vk::ShaderStageFlagBits::eVertex;

            std::vector<VulkanShaderStageCreateInfo> shaders = {};
            shaders.push_back(vertexShader);

            // Set Layouts
            std::vector<vk::DescriptorSetLayout> setLayouts = {};
            setLayouts.push_back(m_frameDescriptor.setLayout);

            // Attachments
            VulkanGraphicsPipelineAttachment depthAttachment;
            depthAttachment.type          = VulkanGraphicsPipelineAttachmentType::Depth;
            depthAttachment.format        = getBackend<VulkanBackend>()->swapchain.depthFormat;
            depthAttachment.loadOp        = vk::AttachmentLoadOp::eClear;
            depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
            depthAttachment.finalLayout   = vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal;

            std::vector<VulkanGraphicsPipelineAttachment> attachments = {};
            attachments.push_back(depthAttachment);

            VulkanGraphicsPipelineCreateInfo createInfo;
            createInfo.bHasVertices             = true;
            createInfo.bHasDepthTest            = true;
            createInfo.bHasDepthWrite           = true;
            createInfo.bHasBlending             = false;
            createInfo.logicalDevice            = getBackend<VulkanBackend>()->logicalDevice;
            createInfo.shaders                  = shaders;
            createInfo.extent                   = getBackend<VulkanBackend>()->swapchain.extent;
            createInfo.descriptorSetLayouts     = setLayouts;
            createInfo.attachments              = attachments;
            createInfo.rasterizaterizationState = rasterizeCreateInfo;

            m_graphicsPipeline = std::make_unique<VulkanGraphicsPipeline>(createInfo);
        }

        void VulkanLSceneShadow::initFramebuffers()
        {
            for (VulkanFrame& frame : getBackend<VulkanBackend>()->swapchain.frames)
            {
                VulkanFrameCreateInfo createInfo;
                createInfo.id            = m_id;
                createInfo.logicalDevice = getBackend<VulkanBackend>()->logicalDevice;
                createInfo.renderPass    = m_graphicsPipeline->renderPass;
                createInfo.extent        = getBackend<VulkanBackend>()->swapchain.extent;
                createInfo.attachments.push_back(frame.shadowImage.view);

                frame.addBuffer(createInfo);
            }
        }
    }
}