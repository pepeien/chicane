#include "Chicane/Renderer/Vulkan/GraphicsPipeline.hpp"

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
                vk::Viewport viewport {};
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
                vk::Rect2D scissor {};
                scissor.offset.x      = 0;
                scissor.offset.y      = 0;
                scissor.extent.width  = inSize.x;
                scissor.extent.height = inSize.y;

                return scissor;
            }

            vk::PipelineShaderStageCreateInfo createVertexShader(const vk::ShaderModule& inShaderModule)
            {
                vk::PipelineShaderStageCreateInfo vertexShaderInfo {};
                vertexShaderInfo.flags  = vk::PipelineShaderStageCreateFlags();
                vertexShaderInfo.stage  = vk::ShaderStageFlagBits::eVertex;
                vertexShaderInfo.module = inShaderModule;
                vertexShaderInfo.pName  = "main";

                return vertexShaderInfo;
            }

            vk::PipelineShaderStageCreateInfo createFragmentShader(const vk::ShaderModule& inShaderModule)
            {
                vk::PipelineShaderStageCreateInfo fragmentShaderInfo {};
                fragmentShaderInfo.flags  = vk::PipelineShaderStageCreateFlags();
                fragmentShaderInfo.stage  = vk::ShaderStageFlagBits::eFragment;
                fragmentShaderInfo.module = inShaderModule;
                fragmentShaderInfo.pName  = "main";

                return fragmentShaderInfo;
            }

            vk::PipelineVertexInputStateCreateInfo createVertexInputState(
                bool bInHasVertices,
                const vk::VertexInputBindingDescription& inBindingDescription,
                const std::vector<vk::VertexInputAttributeDescription>& inAttributeDescriptions
            )
            {
                vk::PipelineVertexInputStateCreateInfo vertexInputInfo {};
                vertexInputInfo.flags = vk::PipelineVertexInputStateCreateFlags();

                if (bInHasVertices)
                {
                    vertexInputInfo.vertexBindingDescriptionCount   = 1;
                    vertexInputInfo.pVertexBindingDescriptions      = &inBindingDescription;
                    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<std::uint32_t>(inAttributeDescriptions.size());
                    vertexInputInfo.pVertexAttributeDescriptions    = inAttributeDescriptions.data();

                    return vertexInputInfo;
                }

                vertexInputInfo.vertexBindingDescriptionCount   = 0;
                vertexInputInfo.pVertexBindingDescriptions      = nullptr;
                vertexInputInfo.vertexAttributeDescriptionCount = 0;
                vertexInputInfo.pVertexAttributeDescriptions    = nullptr;

                return vertexInputInfo;
            }

            vk::PipelineInputAssemblyStateCreateInfo createInputAssemblyState()
            {
                vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo {};
                inputAssemblyInfo.flags    = vk::PipelineInputAssemblyStateCreateFlags();
                inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;

                return inputAssemblyInfo;
            }

            vk::PipelineViewportStateCreateInfo createViewportState(
                const vk::Viewport& inViewport,
                const vk::Rect2D& inScissor
            )
            {
                vk::PipelineViewportStateCreateInfo viewportState {};
                viewportState.flags         = vk::PipelineViewportStateCreateFlags();
                viewportState.viewportCount = 1;
                viewportState.pViewports    = &inViewport;
                viewportState.scissorCount  = 1;
                viewportState.pScissors     = &inScissor;

                return viewportState;
            }

            vk::PipelineDynamicStateCreateInfo createDynamicState(const std::vector<vk::DynamicState>& inDynamicStates)
            {
                vk::PipelineDynamicStateCreateInfo dynamicState {};
                dynamicState.dynamicStateCount = static_cast<std::uint32_t>(inDynamicStates.size());
                dynamicState.pDynamicStates    = inDynamicStates.data();

                return dynamicState;
            }

            vk::PipelineRasterizationStateCreateInfo createRasterizationState(vk::PolygonMode inPolygonMode)
            {
                vk::PipelineRasterizationStateCreateInfo rasterizationState {};
                rasterizationState.flags                   = vk::PipelineRasterizationStateCreateFlags();
                rasterizationState.depthClampEnable        = VK_FALSE;
                rasterizationState.depthBiasEnable         = VK_FALSE;
                rasterizationState.rasterizerDiscardEnable = VK_FALSE;
                rasterizationState.polygonMode             = inPolygonMode;
                rasterizationState.cullMode                = vk::CullModeFlagBits::eNone;
                rasterizationState.frontFace               = vk::FrontFace::eClockwise;
                rasterizationState.lineWidth               = 1.0f;
                rasterizationState.depthBiasConstantFactor = 0.0f;
                rasterizationState.depthBiasClamp          = 0.0f;
                rasterizationState.depthBiasSlopeFactor    = 0.0f;

                return rasterizationState;
            }

            vk::PipelineMultisampleStateCreateInfo createMulitsampleState()
            {
                vk::PipelineMultisampleStateCreateInfo multisampleState {};
                multisampleState.flags                 = vk::PipelineMultisampleStateCreateFlags();
                multisampleState.sampleShadingEnable   = VK_FALSE;
                multisampleState.alphaToCoverageEnable = VK_FALSE;
                multisampleState.alphaToOneEnable      = VK_FALSE;
                multisampleState.rasterizationSamples  = vk::SampleCountFlagBits::e1;
                multisampleState.minSampleShading      = 1.0;

                return multisampleState;
            }

            vk::PipelineColorBlendAttachmentState createBlendAttachmentState()
            {
                vk::PipelineColorBlendAttachmentState attachmentState {};
                attachmentState.colorWriteMask      = vk::ColorComponentFlagBits::eR |
                                                      vk::ColorComponentFlagBits::eG |
                                                      vk::ColorComponentFlagBits::eB |
                                                      vk::ColorComponentFlagBits::eA;
                attachmentState.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
                attachmentState.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
                attachmentState.colorBlendOp        = vk::BlendOp::eAdd;
                attachmentState.srcAlphaBlendFactor = vk::BlendFactor::eOne;
                attachmentState.dstAlphaBlendFactor = vk::BlendFactor::eZero;
                attachmentState.alphaBlendOp        = vk::BlendOp::eAdd;

                return attachmentState;
            }

            vk::PipelineColorBlendStateCreateInfo createColorBlendState()
            {
                vk::PipelineColorBlendStateCreateInfo colorBlendState {};
                colorBlendState.flags             = vk::PipelineColorBlendStateCreateFlags();
                colorBlendState.logicOpEnable     = VK_FALSE;
                colorBlendState.logicOp           = vk::LogicOp::eCopy;
                colorBlendState.blendConstants[0] = 0.0f;
                colorBlendState.blendConstants[1] = 0.0f;
                colorBlendState.blendConstants[2] = 0.0f;
                colorBlendState.blendConstants[3] = 0.0f;

                return colorBlendState;
            }

            vk::PipelineDepthStencilStateCreateInfo createDepthStencilState()
            {
                vk::PipelineDepthStencilStateCreateInfo depthStencilCreateInfo {};
                depthStencilCreateInfo.flags                 = vk::PipelineDepthStencilStateCreateFlags();
                depthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
                depthStencilCreateInfo.stencilTestEnable     = VK_FALSE;
                depthStencilCreateInfo.depthCompareOp        = vk::CompareOp::eLess;

                return depthStencilCreateInfo;
            }

            vk::PipelineLayout createLayout(
                const std::vector<vk::DescriptorSetLayout>& inDescriptorSetLayouts,
                const std::vector<vk::PushConstantRange>& inPushConstantRanges,
                const vk::Device& inLogicalDevice
            )
            {
                vk::PipelineLayoutCreateInfo layoutCreateInfo {};
                layoutCreateInfo.flags                  = vk::PipelineLayoutCreateFlags();
                layoutCreateInfo.setLayoutCount         = static_cast<std::uint32_t>(inDescriptorSetLayouts.size());
                layoutCreateInfo.pSetLayouts            = inDescriptorSetLayouts.data();
                layoutCreateInfo.pushConstantRangeCount = static_cast<std::uint32_t>(inPushConstantRanges.size());
                layoutCreateInfo.pPushConstantRanges    = inPushConstantRanges.data();

                return inLogicalDevice.createPipelineLayout(layoutCreateInfo);
            }

            vk::AttachmentDescription createColorAttachment(const Attachment& inAttachment)
            {
                vk::AttachmentDescription attachmentDescription {};
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

            vk::AttachmentReference createColorAttachmentRef()
            {
                vk::AttachmentReference attachmentRef {};
                attachmentRef.attachment = 0;
                attachmentRef.layout     = vk::ImageLayout::eColorAttachmentOptimal;

                return attachmentRef;
            }

            vk::SubpassDependency createColorSubpassDepedency()
            {
                vk::SubpassDependency subpassDepedency {};
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
                vk::AttachmentDescription attachmentDescription {};
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

            vk::AttachmentReference createDepthAttachmentRef()
            {
                vk::AttachmentReference attachmentRef {};
                attachmentRef.attachment = 1;
                attachmentRef.layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal;

                return attachmentRef;
            }

            vk::SubpassDependency createDepthSubpassDepedency()
            {
                vk::SubpassDependency subpassDepedency {};
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
                const vk::Device& inLogicalDevice
            )
            {
                std::vector<vk::AttachmentReference> colorAttachmentRefs;
                colorAttachmentRefs.push_back(createColorAttachmentRef());

                std::vector<vk::SubpassDependency> subpassDependecies;   
                subpassDependecies.push_back(createColorSubpassDepedency());

                vk::SubpassDescription subpass {};
                subpass.flags                   = vk::SubpassDescriptionFlags();
                subpass.pipelineBindPoint       = vk::PipelineBindPoint::eGraphics;
                subpass.colorAttachmentCount    = static_cast<std::uint32_t>(colorAttachmentRefs.size());
                subpass.pColorAttachments       = colorAttachmentRefs.data();

                vk::AttachmentReference depthAttachmentRef;

                if (inAttachments.size() > 1)
                {
                    subpassDependecies.push_back(createDepthSubpassDepedency());

                    depthAttachmentRef = createDepthAttachmentRef();

                    subpass.pDepthStencilAttachment = &depthAttachmentRef;
                }
        
                vk::RenderPassCreateInfo renderPassInfo {};
                renderPassInfo.flags           = vk::RenderPassCreateFlags();
                renderPassInfo.attachmentCount = static_cast<std::uint32_t>(inAttachments.size());
                renderPassInfo.pAttachments    = inAttachments.data();
                renderPassInfo.dependencyCount = static_cast<std::uint32_t>(subpassDependecies.size());
                renderPassInfo.pDependencies   = subpassDependecies.data();
                renderPassInfo.subpassCount    = 1;
                renderPassInfo.pSubpasses      = &subpass;
        
                return inLogicalDevice.createRenderPass(renderPassInfo);
            }
        }
    }
}