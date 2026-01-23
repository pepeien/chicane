#include "Chicane/Renderer/Backend/Vulkan/GraphicsPipeline.hpp"

#include "Chicane/Renderer/Backend/Vulkan/Shader.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Shader/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Vertex.hpp"

namespace Chicane
{
    namespace Renderer
    {
        vk::Viewport VulkanGraphicsPipeline::createViewport(
            const Vec<2, std::uint32_t>& inSize, const Vec2& inPosition
        )
        {
            vk::Viewport viewport;
            viewport.x        = inPosition.x;
            viewport.y        = inPosition.y;
            viewport.width    = static_cast<float>(inSize.x);
            viewport.height   = static_cast<float>(inSize.y);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;

            return viewport;
        }

        vk::Rect2D VulkanGraphicsPipeline::createScissor(const Vec<2, std::uint32_t>& inSize)
        {
            vk::Rect2D scissor;
            scissor.offset.x      = 0;
            scissor.offset.y      = 0;
            scissor.extent.width  = inSize.x;
            scissor.extent.height = inSize.y;

            return scissor;
        }

        vk::PipelineVertexInputStateCreateInfo VulkanGraphicsPipeline::createVertexInputState()
        {
            vk::PipelineVertexInputStateCreateInfo createInfo;
            createInfo.flags                           = vk::PipelineVertexInputStateCreateFlags();
            createInfo.vertexBindingDescriptionCount   = 0;
            createInfo.pVertexBindingDescriptions      = nullptr;
            createInfo.vertexAttributeDescriptionCount = 0;
            createInfo.pVertexAttributeDescriptions    = nullptr;

            return createInfo;
        }

        vk::PipelineInputAssemblyStateCreateInfo VulkanGraphicsPipeline::createInputAssemblyState()
        {
            vk::PipelineInputAssemblyStateCreateInfo createInfo;
            createInfo.flags    = vk::PipelineInputAssemblyStateCreateFlags();
            createInfo.topology = vk::PrimitiveTopology::eTriangleList;

            return createInfo;
        }

        vk::PipelineViewportStateCreateInfo VulkanGraphicsPipeline::createViewportState(
            const vk::Viewport& inViewport, const vk::Rect2D& inScissor
        )
        {
            vk::PipelineViewportStateCreateInfo createInfo;
            createInfo.flags         = vk::PipelineViewportStateCreateFlags();
            createInfo.viewportCount = 1;
            createInfo.pViewports    = &inViewport;
            createInfo.scissorCount  = 1;
            createInfo.pScissors     = &inScissor;

            return createInfo;
        }

        vk::PipelineDynamicStateCreateInfo VulkanGraphicsPipeline::createDynamicState(
            const std::vector<vk::DynamicState>& inDynamicStates
        )
        {
            vk::PipelineDynamicStateCreateInfo createInfo;
            createInfo.dynamicStateCount = static_cast<std::uint32_t>(inDynamicStates.size());
            createInfo.pDynamicStates    = inDynamicStates.data();

            return createInfo;
        }

        vk::PipelineRasterizationStateCreateInfo VulkanGraphicsPipeline::createRasterizationState(
            vk::PolygonMode inPolygonMode
        )
        {
            vk::PipelineRasterizationStateCreateInfo createInfo;
            createInfo.flags                   = vk::PipelineRasterizationStateCreateFlags();
            createInfo.depthClampEnable        = VK_FALSE;
            createInfo.depthBiasEnable         = VK_FALSE;
            createInfo.rasterizerDiscardEnable = VK_FALSE;
            createInfo.polygonMode             = inPolygonMode;
            createInfo.cullMode                = vk::CullModeFlagBits::eBack;
            createInfo.frontFace               = vk::FrontFace::eCounterClockwise;
            createInfo.lineWidth               = 1.0f;
            createInfo.depthBiasConstantFactor = 0.0f;
            createInfo.depthBiasClamp          = 0.0f;
            createInfo.depthBiasSlopeFactor    = 0.0f;

            return createInfo;
        }

        vk::PipelineMultisampleStateCreateInfo VulkanGraphicsPipeline::createMulitsampleState()
        {
            vk::PipelineMultisampleStateCreateInfo createInfo;
            createInfo.flags                 = vk::PipelineMultisampleStateCreateFlags();
            createInfo.sampleShadingEnable   = VK_FALSE;
            createInfo.alphaToCoverageEnable = VK_FALSE;
            createInfo.alphaToOneEnable      = VK_FALSE;
            createInfo.rasterizationSamples  = vk::SampleCountFlagBits::e1;
            createInfo.minSampleShading      = 1.0;

            return createInfo;
        }

        vk::PipelineColorBlendAttachmentState VulkanGraphicsPipeline::createBlendAttachmentState()
        {
            vk::PipelineColorBlendAttachmentState createInfo;
            createInfo.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
            createInfo.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
            createInfo.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
            createInfo.colorBlendOp        = vk::BlendOp::eAdd;
            createInfo.srcAlphaBlendFactor = vk::BlendFactor::eOne;
            createInfo.dstAlphaBlendFactor = vk::BlendFactor::eZero;
            createInfo.alphaBlendOp        = vk::BlendOp::eAdd;

            return createInfo;
        }

        vk::PipelineColorBlendStateCreateInfo VulkanGraphicsPipeline::createColorBlendState()
        {
            vk::PipelineColorBlendStateCreateInfo createInfo;
            createInfo.flags             = vk::PipelineColorBlendStateCreateFlags();
            createInfo.logicOpEnable     = VK_FALSE;
            createInfo.logicOp           = vk::LogicOp::eCopy;
            createInfo.blendConstants[0] = 0.0f;
            createInfo.blendConstants[1] = 0.0f;
            createInfo.blendConstants[2] = 0.0f;
            createInfo.blendConstants[3] = 0.0f;

            return createInfo;
        }

        vk::PipelineDepthStencilStateCreateInfo VulkanGraphicsPipeline::createDepthStencilState()
        {
            vk::PipelineDepthStencilStateCreateInfo createInfo;
            createInfo.flags                 = vk::PipelineDepthStencilStateCreateFlags();
            createInfo.depthBoundsTestEnable = VK_FALSE;
            createInfo.stencilTestEnable     = VK_FALSE;
            createInfo.depthCompareOp        = vk::CompareOp::eLess;
            createInfo.minDepthBounds        = 0.0f;
            createInfo.maxDepthBounds        = 1.0f;

            return createInfo;
        }

        vk::PipelineLayout VulkanGraphicsPipeline::createLayout(
            const std::vector<vk::DescriptorSetLayout>& inDescriptorSetLayouts,
            const std::vector<vk::PushConstantRange>&   inPushConstants,
            const vk::Device&                           inLogicalDevice
        )
        {
            vk::PipelineLayoutCreateInfo createInfo;
            createInfo.flags                  = vk::PipelineLayoutCreateFlags();
            createInfo.setLayoutCount         = static_cast<std::uint32_t>(inDescriptorSetLayouts.size());
            createInfo.pSetLayouts            = inDescriptorSetLayouts.data();
            createInfo.pushConstantRangeCount = static_cast<std::uint32_t>(inPushConstants.size());
            createInfo.pPushConstantRanges    = inPushConstants.data();

            return inLogicalDevice.createPipelineLayout(createInfo);
        }

        vk::AttachmentDescription VulkanGraphicsPipeline::createColorAttachment(
            const VulkanGraphicsPipelineAttachment& inAttachment
        )
        {
            vk::AttachmentDescription createInfo;
            createInfo.flags          = vk::AttachmentDescriptionFlags();
            createInfo.format         = inAttachment.format;
            createInfo.samples        = vk::SampleCountFlagBits::e1;
            createInfo.loadOp         = inAttachment.loadOp;
            createInfo.storeOp        = vk::AttachmentStoreOp::eStore;
            createInfo.stencilLoadOp  = vk::AttachmentLoadOp::eLoad;
            createInfo.stencilStoreOp = vk::AttachmentStoreOp::eStore;
            createInfo.initialLayout  = inAttachment.initialLayout;
            createInfo.finalLayout    = inAttachment.finalLayout;

            return createInfo;
        }

        vk::SubpassDependency VulkanGraphicsPipeline::createColorSubpassDepedency()
        {
            vk::SubpassDependency subpassDepedency;
            subpassDepedency.srcSubpass    = VK_SUBPASS_EXTERNAL;
            subpassDepedency.dstSubpass    = 0;
            subpassDepedency.srcStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            subpassDepedency.dstStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            subpassDepedency.srcAccessMask = vk::AccessFlagBits::eNone;
            subpassDepedency.dstAccessMask =
                vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;

            return subpassDepedency;
        }

        vk::AttachmentDescription VulkanGraphicsPipeline::createDepthAttachment(
            const VulkanGraphicsPipelineAttachment& inAttachment
        )
        {
            vk::AttachmentDescription attachmentDescription;
            attachmentDescription.flags          = vk::AttachmentDescriptionFlags();
            attachmentDescription.format         = inAttachment.format;
            attachmentDescription.samples        = vk::SampleCountFlagBits::e1;
            attachmentDescription.loadOp         = inAttachment.loadOp;
            attachmentDescription.storeOp        = vk::AttachmentStoreOp::eStore;
            attachmentDescription.stencilLoadOp  = vk::AttachmentLoadOp::eLoad;
            attachmentDescription.stencilStoreOp = vk::AttachmentStoreOp::eStore;
            attachmentDescription.initialLayout  = inAttachment.initialLayout;
            attachmentDescription.finalLayout    = inAttachment.finalLayout;

            return attachmentDescription;
        }

        vk::SubpassDependency VulkanGraphicsPipeline::createDepthSubpassDepedency()
        {
            vk::SubpassDependency subpassDepedency;
            subpassDepedency.srcSubpass   = VK_SUBPASS_EXTERNAL;
            subpassDepedency.dstSubpass   = 0;
            subpassDepedency.srcStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests |
                                            vk::PipelineStageFlagBits::eLateFragmentTests;
            subpassDepedency.dstStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests |
                                            vk::PipelineStageFlagBits::eLateFragmentTests;
            subpassDepedency.srcAccessMask = vk::AccessFlagBits::eNone;
            subpassDepedency.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead |
                                             vk::AccessFlagBits::eDepthStencilAttachmentWrite;

            return subpassDepedency;
        }

        vk::RenderPass VulkanGraphicsPipeline::createRendepass(
            const std::vector<vk::AttachmentDescription>& inAttachments,
            const vk::Device&                             inLogicalDevice,
            bool                                          bInHasColor,
            bool                                          bInHasDepth
        )
        {
            std::vector<vk::SubpassDependency> subpassDependecies;

            vk::SubpassDescription subpass;
            subpass.flags             = vk::SubpassDescriptionFlags();
            subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;

            vk::AttachmentReference colorAttachmentRef;
            if (bInHasColor)
            {
                colorAttachmentRef.attachment = 0;
                colorAttachmentRef.layout     = vk::ImageLayout::eColorAttachmentOptimal;

                subpass.colorAttachmentCount = 1;
                subpass.pColorAttachments    = &colorAttachmentRef;

                subpassDependecies.push_back(createColorSubpassDepedency());
            }

            vk::AttachmentReference depthAttachmentRef;
            if (bInHasDepth)
            {
                depthAttachmentRef.attachment = bInHasColor ? 1 : 0;
                depthAttachmentRef.layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal;

                subpass.pDepthStencilAttachment = &depthAttachmentRef;

                subpassDependecies.push_back(createDepthSubpassDepedency());
            }

            vk::RenderPassCreateInfo createInfo;
            createInfo.flags           = vk::RenderPassCreateFlags();
            createInfo.attachmentCount = static_cast<std::uint32_t>(inAttachments.size());
            createInfo.pAttachments    = inAttachments.data();
            createInfo.dependencyCount = static_cast<std::uint32_t>(subpassDependecies.size());
            createInfo.pDependencies   = subpassDependecies.data();
            createInfo.subpassCount    = 1;
            createInfo.pSubpasses      = &subpass;

            return inLogicalDevice.createRenderPass(createInfo);
        }

        VulkanGraphicsPipeline::VulkanGraphicsPipeline(const VulkanGraphicsPipelineCreateInfo& inCreateInfo)
            : m_logicalDevice(inCreateInfo.logicalDevice)
        {
            // Vertex Input
            vk::PipelineVertexInputStateCreateInfo vertexInputState = createVertexInputState();

            vk::VertexInputBindingDescription bindingDescription = VulkanVertex::getBindingDescription();
            std::vector<vk::VertexInputAttributeDescription> attributeDescriptions =
                VulkanVertex::getAttributeDescriptions();

            if (inCreateInfo.bHasVertices)
            {
                vertexInputState.vertexBindingDescriptionCount = 1;
                vertexInputState.pVertexBindingDescriptions    = &bindingDescription;
                vertexInputState.vertexAttributeDescriptionCount =
                    static_cast<std::uint32_t>(attributeDescriptions.size());
                vertexInputState.pVertexAttributeDescriptions = attributeDescriptions.data();
            }

            // Input Assembly
            vk::PipelineInputAssemblyStateCreateInfo inputAsstembyState = createInputAssemblyState();

            // Viewport
            Vec<2, std::uint32_t> size(inCreateInfo.extent.width, inCreateInfo.extent.height);

            vk::PipelineViewportStateCreateInfo viewportState =
                createViewportState(createViewport(size), createScissor(size));

            // Dynamic State
            std::vector<vk::DynamicState> dynamicStates = {
                vk::DynamicState::eViewport,
                vk::DynamicState::eScissor
            };
            vk::PipelineDynamicStateCreateInfo dynamicState = createDynamicState(dynamicStates);

            // Shader Stage
            std::vector<vk::PipelineShaderStageCreateInfo> shaders;
            for (const VulkanShaderStageCreateInfo& shader : inCreateInfo.shaders)
            {
                shaders.push_back(VulkanShader::initShaderStage(m_logicalDevice, shader));
            }

            // Sampling
            vk::PipelineMultisampleStateCreateInfo multisampleState = createMulitsampleState();

            // Color Blending
            vk::PipelineColorBlendAttachmentState colorBlendAttachmentState = createBlendAttachmentState();
            colorBlendAttachmentState.blendEnable = inCreateInfo.bHasBlending ? VK_TRUE : VK_FALSE;

            vk::PipelineColorBlendStateCreateInfo colorBlendState = createColorBlendState();
            colorBlendState.attachmentCount                       = 1;
            colorBlendState.pAttachments                          = &colorBlendAttachmentState;

            // Depthning
            vk::PipelineDepthStencilStateCreateInfo depthStencilState = createDepthStencilState();
            depthStencilState.depthTestEnable  = inCreateInfo.bHasDepthTest ? VK_TRUE : VK_FALSE;
            depthStencilState.depthWriteEnable = inCreateInfo.bHasDepthWrite ? VK_TRUE : VK_FALSE;

            // Attachment
            std::vector<vk::AttachmentDescription> attachments;
            for (const VulkanGraphicsPipelineAttachment& attachment : inCreateInfo.attachments)
            {
                if (attachment.type == VulkanGraphicsPipelineAttachmentType::Color)
                {
                    attachments.push_back(createColorAttachment(attachment));
                }

                if (attachment.type == VulkanGraphicsPipelineAttachmentType::Depth)
                {
                    attachments.push_back(createDepthAttachment(attachment));
                }
            }

            vk::GraphicsPipelineCreateInfo pipelineInfo{};
            pipelineInfo.flags               = vk::PipelineCreateFlags();
            pipelineInfo.pVertexInputState   = &vertexInputState;
            pipelineInfo.pInputAssemblyState = &inputAsstembyState;
            pipelineInfo.pViewportState      = &viewportState;
            pipelineInfo.pDynamicState       = &dynamicState;
            pipelineInfo.stageCount          = static_cast<std::uint32_t>(shaders.size());
            pipelineInfo.pStages             = shaders.data();
            pipelineInfo.pRasterizationState = &inCreateInfo.rasterizaterizationState;
            pipelineInfo.pMultisampleState   = &multisampleState;
            pipelineInfo.pColorBlendState    = &colorBlendState;
            pipelineInfo.pDepthStencilState  = &depthStencilState;
            pipelineInfo.layout              = createLayout(
                inCreateInfo.descriptorSetLayouts,
                inCreateInfo.pushConstantRanges,
                m_logicalDevice
            );
            pipelineInfo.renderPass = createRendepass(
                attachments,
                m_logicalDevice,
                std::find_if(
                    inCreateInfo.attachments.begin(),
                    inCreateInfo.attachments.end(),
                    [](const auto& inAttachment) {
                        return inAttachment.type == VulkanGraphicsPipelineAttachmentType::Color;
                    }
                ) != inCreateInfo.attachments.end(),
                std::find_if(
                    inCreateInfo.attachments.begin(),
                    inCreateInfo.attachments.end(),
                    [](const auto& inAttachment) {
                        return inAttachment.type == VulkanGraphicsPipelineAttachmentType::Depth;
                    }
                ) != inCreateInfo.attachments.end()
            );
            pipelineInfo.subpass            = 0;
            pipelineInfo.basePipelineHandle = nullptr;

            layout     = pipelineInfo.layout;
            renderPass = pipelineInfo.renderPass;
            instance   = m_logicalDevice.createGraphicsPipeline(nullptr, pipelineInfo).value;

            for (vk::PipelineShaderStageCreateInfo& shader : shaders)
            {
                m_logicalDevice.destroyShaderModule(shader.module);
            }
        }

        VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
        {
            m_logicalDevice.waitIdle();

            m_logicalDevice.destroyPipeline(instance);
            m_logicalDevice.destroyPipelineLayout(layout);
            m_logicalDevice.destroyRenderPass(renderPass);
        }

        void VulkanGraphicsPipeline::bind(vk::CommandBuffer& inCommandBuffer)
        {
            inCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, instance);
        }

        void VulkanGraphicsPipeline::bindDescriptorSet(
            vk::CommandBuffer& inCommandBuffer, std::uint32_t inIndex, vk::DescriptorSet inDescriptorSet
        )
        {
            inCommandBuffer.bindDescriptorSets(
                vk::PipelineBindPoint::eGraphics,
                layout,
                inIndex,
                inDescriptorSet,
                nullptr
            );
        }
    }
}