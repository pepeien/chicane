#include "Runtime/Renderer/Vulkan/GraphicsPipeline.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace GraphicsPipeline
        {
            vk::Viewport createViewport(
                const Vec<2, std::uint32_t>& inSize,
                const Vec<2, float>& inPosition
            )
            {
                vk::Viewport viewport = {};
                viewport.x        = inPosition.x;
                viewport.y        = inPosition.y;
                viewport.width    = static_cast<float>(inSize.x);
                viewport.height   = static_cast<float>(inSize.y);
                viewport.minDepth = 0.0f;
                viewport.maxDepth = 1.0f;

                return viewport;
            }

            vk::Rect2D createScissor(const Vec<2, std::uint32_t>& inSize)
            {
                vk::Rect2D scissor = {};
                scissor.offset.x      = 0;
                scissor.offset.y      = 0;
                scissor.extent.width  = inSize.x;
                scissor.extent.height = inSize.y;

                return scissor;
            }

            vk::PipelineVertexInputStateCreateInfo createVertexInputState()
            {
                vk::PipelineVertexInputStateCreateInfo createInfo = {};
                createInfo.flags                           = vk::PipelineVertexInputStateCreateFlags();
                createInfo.vertexBindingDescriptionCount   = 0;
                createInfo.pVertexBindingDescriptions      = nullptr;
                createInfo.vertexAttributeDescriptionCount = 0;
                createInfo.pVertexAttributeDescriptions    = nullptr;

                return createInfo;
            }

            vk::PipelineInputAssemblyStateCreateInfo createInputAssemblyState()
            {
                vk::PipelineInputAssemblyStateCreateInfo createInfo = {};
                createInfo.flags    = vk::PipelineInputAssemblyStateCreateFlags();
                createInfo.topology = vk::PrimitiveTopology::eTriangleList;

                return createInfo;
            }

            vk::PipelineViewportStateCreateInfo createViewportState(
                const vk::Viewport& inViewport,
                const vk::Rect2D& inScissor
            )
            {
                vk::PipelineViewportStateCreateInfo createInfo = {};
                createInfo.flags         = vk::PipelineViewportStateCreateFlags();
                createInfo.viewportCount = 1;
                createInfo.pViewports    = &inViewport;
                createInfo.scissorCount  = 1;
                createInfo.pScissors     = &inScissor;

                return createInfo;
            }

            vk::PipelineDynamicStateCreateInfo createDynamicState(const std::vector<vk::DynamicState>& inDynamicStates)
            {
                vk::PipelineDynamicStateCreateInfo createInfo = {};
                createInfo.dynamicStateCount = static_cast<std::uint32_t>(inDynamicStates.size());
                createInfo.pDynamicStates    = inDynamicStates.data();

                return createInfo;
            }

            vk::PipelineRasterizationStateCreateInfo createRasterizationState(vk::PolygonMode inPolygonMode)
            {
                vk::PipelineRasterizationStateCreateInfo createInfo = {};
                createInfo.flags                   = vk::PipelineRasterizationStateCreateFlags();
                createInfo.depthClampEnable        = VK_FALSE;
                createInfo.depthBiasEnable         = VK_FALSE;
                createInfo.rasterizerDiscardEnable = VK_FALSE;
                createInfo.polygonMode             = inPolygonMode;
                createInfo.cullMode                = vk::CullModeFlagBits::eNone;
                createInfo.frontFace               = vk::FrontFace::eClockwise;
                createInfo.lineWidth               = 1.0f;
                createInfo.depthBiasConstantFactor = 0.0f;
                createInfo.depthBiasClamp          = 0.0f;
                createInfo.depthBiasSlopeFactor    = 0.0f;

                return createInfo;
            }

            vk::PipelineMultisampleStateCreateInfo createMulitsampleState()
            {
                vk::PipelineMultisampleStateCreateInfo createInfo = {};
                createInfo.flags                 = vk::PipelineMultisampleStateCreateFlags();
                createInfo.sampleShadingEnable   = VK_FALSE;
                createInfo.alphaToCoverageEnable = VK_FALSE;
                createInfo.alphaToOneEnable      = VK_FALSE;
                createInfo.rasterizationSamples  = vk::SampleCountFlagBits::e1;
                createInfo.minSampleShading      = 1.0;

                return createInfo;
            }

            vk::PipelineColorBlendAttachmentState createBlendAttachmentState()
            {
                vk::PipelineColorBlendAttachmentState createInfo = {};
                createInfo.colorWriteMask      = vk::ColorComponentFlagBits::eR |
                                                 vk::ColorComponentFlagBits::eG |
                                                 vk::ColorComponentFlagBits::eB |
                                                 vk::ColorComponentFlagBits::eA;
                createInfo.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
                createInfo.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
                createInfo.colorBlendOp        = vk::BlendOp::eAdd;
                createInfo.srcAlphaBlendFactor = vk::BlendFactor::eOne;
                createInfo.dstAlphaBlendFactor = vk::BlendFactor::eZero;
                createInfo.alphaBlendOp        = vk::BlendOp::eAdd;

                return createInfo;
            }

            vk::PipelineColorBlendStateCreateInfo createColorBlendState()
            {
                vk::PipelineColorBlendStateCreateInfo createInfo = {};
                createInfo.flags             = vk::PipelineColorBlendStateCreateFlags();
                createInfo.logicOpEnable     = VK_FALSE;
                createInfo.logicOp           = vk::LogicOp::eCopy;
                createInfo.blendConstants[0] = 0.0f;
                createInfo.blendConstants[1] = 0.0f;
                createInfo.blendConstants[2] = 0.0f;
                createInfo.blendConstants[3] = 0.0f;

                return createInfo;
            }

            vk::PipelineDepthStencilStateCreateInfo createDepthStencilState()
            {
                vk::PipelineDepthStencilStateCreateInfo createInfo = {};
                createInfo.flags                 = vk::PipelineDepthStencilStateCreateFlags();
                createInfo.depthBoundsTestEnable = VK_FALSE;
                createInfo.stencilTestEnable     = VK_FALSE;
                createInfo.depthCompareOp        = vk::CompareOp::eLess;

                return createInfo;
            }

            vk::PipelineLayout createLayout(const std::vector<vk::DescriptorSetLayout>& inDescriptorSetLayouts,const vk::Device& inLogicalDevice)
            {
                vk::PipelineLayoutCreateInfo createInfo = {};
                createInfo.flags                  = vk::PipelineLayoutCreateFlags();
                createInfo.setLayoutCount         = static_cast<std::uint32_t>(inDescriptorSetLayouts.size());
                createInfo.pSetLayouts            = inDescriptorSetLayouts.data();
                createInfo.pushConstantRangeCount = 0;
                createInfo.pPushConstantRanges    = nullptr;

                return inLogicalDevice.createPipelineLayout(createInfo);
            }

            vk::AttachmentDescription createColorAttachment(const Attachment& inAttachment)
            {
                vk::AttachmentDescription createInfo = {};
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

            vk::SubpassDependency createColorSubpassDepedency()
            {
                vk::SubpassDependency subpassDepedency = {};
                subpassDepedency.srcSubpass    = VK_SUBPASS_EXTERNAL;
                subpassDepedency.dstSubpass    = 0;
                subpassDepedency.srcStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
                subpassDepedency.dstStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
                subpassDepedency.srcAccessMask = vk::AccessFlagBits::eNone;
                subpassDepedency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead |
                                                 vk::AccessFlagBits::eColorAttachmentWrite;

                return subpassDepedency;
            }

            vk::AttachmentDescription createDepthAttachment(const Attachment& inAttachment)
            {
                vk::AttachmentDescription attachmentDescription = {};
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

            vk::SubpassDependency createDepthSubpassDepedency()
            {
                vk::SubpassDependency subpassDepedency = {};
                subpassDepedency.srcSubpass    = VK_SUBPASS_EXTERNAL;
                subpassDepedency.dstSubpass    = 0;
                subpassDepedency.srcStageMask  = vk::PipelineStageFlagBits::eEarlyFragmentTests;
                subpassDepedency.dstStageMask  = vk::PipelineStageFlagBits::eEarlyFragmentTests;
                subpassDepedency.srcAccessMask = vk::AccessFlagBits::eNone;  
                subpassDepedency.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead |
                                                 vk::AccessFlagBits::eDepthStencilAttachmentWrite;

                return subpassDepedency;
            }

            vk::RenderPass createRendepass(
                const std::vector<vk::AttachmentDescription>& inAttachments,
                const vk::Device& inLogicalDevice,
                bool bInHasColor,
                bool bInHasDepth
            )
            {
                std::vector<vk::SubpassDependency> subpassDependecies = {};

                vk::SubpassDescription subpass = {};
                subpass.flags             = vk::SubpassDescriptionFlags();
                subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;

                vk::AttachmentReference colorAttachmentRef = {};
                if (bInHasColor)
                {
                    colorAttachmentRef.attachment = 0;
                    colorAttachmentRef.layout     = vk::ImageLayout::eColorAttachmentOptimal;

                    subpass.colorAttachmentCount = 1;
                    subpass.pColorAttachments    = &colorAttachmentRef;

                    subpassDependecies.push_back(createColorSubpassDepedency());
                }

                vk::AttachmentReference depthAttachmentRef = {};
                if (bInHasDepth)
                {
                    depthAttachmentRef.attachment = bInHasColor ? 1 : 0;
                    depthAttachmentRef.layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal;

                    subpass.pDepthStencilAttachment = &depthAttachmentRef;

                    subpassDependecies.push_back(createDepthSubpassDepedency());
                }
        
                vk::RenderPassCreateInfo createInfo = {};
                createInfo.flags           = vk::RenderPassCreateFlags();
                createInfo.attachmentCount = static_cast<std::uint32_t>(inAttachments.size());
                createInfo.pAttachments    = inAttachments.data();
                createInfo.dependencyCount = static_cast<std::uint32_t>(subpassDependecies.size());
                createInfo.pDependencies   = subpassDependecies.data();
                createInfo.subpassCount    = 1;
                createInfo.pSubpasses      = &subpass;
        
                return inLogicalDevice.createRenderPass(createInfo);
            }
        }
    }
}