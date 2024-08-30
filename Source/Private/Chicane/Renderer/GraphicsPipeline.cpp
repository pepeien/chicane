#include "Chicane/Renderer/GraphicsPipeline.hpp"

namespace Chicane
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
            bool isHasVertices,
            const vk::VertexInputBindingDescription& inBindingDescription,
            const std::vector<vk::VertexInputAttributeDescription>& inAttributeDescriptions
        )
        {
            vk::PipelineVertexInputStateCreateInfo vertexInputInfo {};
            vertexInputInfo.flags = vk::PipelineVertexInputStateCreateFlags();

            if (isHasVertices)
            {
                vertexInputInfo.vertexBindingDescriptionCount   = 1;
                vertexInputInfo.pVertexBindingDescriptions      = &inBindingDescription;
                vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(inAttributeDescriptions.size());
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
            dynamicState.dynamicStateCount = static_cast<uint32_t>(inDynamicStates.size());
            dynamicState.pDynamicStates    = inDynamicStates.data();

            return dynamicState;
        }

        vk::PipelineRasterizationStateCreateInfo createRasterizationState()
        {
            vk::PipelineRasterizationStateCreateInfo rasterizationState {};
            rasterizationState.flags                   = vk::PipelineRasterizationStateCreateFlags();
            rasterizationState.depthClampEnable        = VK_FALSE;
            rasterizationState.depthBiasEnable         = VK_FALSE;
            rasterizationState.rasterizerDiscardEnable = VK_FALSE;
            rasterizationState.polygonMode             = vk::PolygonMode::eFill;
            rasterizationState.cullMode                = vk::CullModeFlagBits::eBack;
            rasterizationState.frontFace               = vk::FrontFace::eCounterClockwise;
            rasterizationState.lineWidth               = 1.0f;

            return rasterizationState;
        }

        vk::PipelineMultisampleStateCreateInfo createMulitsampleState()
        {
            vk::PipelineMultisampleStateCreateInfo multisampleState {};
            multisampleState.flags                = vk::PipelineMultisampleStateCreateFlags();
            multisampleState.sampleShadingEnable  = VK_FALSE;
            multisampleState.rasterizationSamples = vk::SampleCountFlagBits::e1;

            return multisampleState;
        }

        vk::PipelineColorBlendStateCreateInfo createColorBlendState(
            const vk::PipelineColorBlendAttachmentState& colorBlendAttachmentState
        )
        {
            vk::PipelineColorBlendStateCreateInfo colorBlendState {};
            colorBlendState.flags             = vk::PipelineColorBlendStateCreateFlags();
            colorBlendState.logicOpEnable     = VK_FALSE;
            colorBlendState.logicOp           = vk::LogicOp::eCopy;
            colorBlendState.attachmentCount   = 1;
            colorBlendState.pAttachments      = &colorBlendAttachmentState;
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
            depthStencilCreateInfo.depthTestEnable       = VK_TRUE;
            depthStencilCreateInfo.depthWriteEnable      = VK_TRUE;
            depthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
            depthStencilCreateInfo.stencilTestEnable     = VK_FALSE;
            depthStencilCreateInfo.depthCompareOp        = vk::CompareOp::eLess;
            depthStencilCreateInfo.minDepthBounds        = 0.0f;
            depthStencilCreateInfo.maxDepthBounds        = 1.0f;

            return depthStencilCreateInfo;
        }

        vk::PipelineLayout createLayout(
            const std::vector<vk::DescriptorSetLayout>& inDescriptorSetLayouts,
            const vk::Device& inLogicalDevice
        )
        {
            vk::PipelineLayoutCreateInfo layoutCreateInfo {};
            layoutCreateInfo.flags          = vk::PipelineLayoutCreateFlags();
            layoutCreateInfo.setLayoutCount = static_cast<uint32_t>(inDescriptorSetLayouts.size());
            layoutCreateInfo.pSetLayouts    = inDescriptorSetLayouts.data();

            return inLogicalDevice.createPipelineLayout(layoutCreateInfo);
        }

        vk::AttachmentDescription createColorAttachment(
            vk::Format inFormat,
            vk::AttachmentLoadOp inLoadOp,
            vk::ImageLayout inInitialLayout
        )
        {
            vk::AttachmentDescription attachmentDescription {};
            attachmentDescription.flags          = vk::AttachmentDescriptionFlags();
            attachmentDescription.format         = inFormat;
            attachmentDescription.samples        = vk::SampleCountFlagBits::e1;
            attachmentDescription.loadOp         = inLoadOp;
            attachmentDescription.storeOp        = vk::AttachmentStoreOp::eStore;
            attachmentDescription.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
            attachmentDescription.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
            attachmentDescription.initialLayout  = inInitialLayout;
            attachmentDescription.finalLayout    = vk::ImageLayout::ePresentSrcKHR;

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
            subpassDepedency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

            return subpassDepedency;
        }

        vk::AttachmentDescription createDepthAttachment(const vk::Format& inFormat)
        {
            vk::AttachmentDescription attachmentDescription {};
            attachmentDescription.flags          = vk::AttachmentDescriptionFlags();
            attachmentDescription.format         = inFormat;
            attachmentDescription.samples        = vk::SampleCountFlagBits::e1;
            attachmentDescription.loadOp         = vk::AttachmentLoadOp::eClear;
            attachmentDescription.storeOp        = vk::AttachmentStoreOp::eDontCare;
            attachmentDescription.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
            attachmentDescription.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
            attachmentDescription.initialLayout  = vk::ImageLayout::eUndefined;
            attachmentDescription.finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    
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
            subpassDepedency.srcStageMask  = vk::PipelineStageFlagBits::eEarlyFragmentTests |
                                             vk::PipelineStageFlagBits::eLateFragmentTests;
            subpassDepedency.dstStageMask  = vk::PipelineStageFlagBits::eEarlyFragmentTests |
                                             vk::PipelineStageFlagBits::eLateFragmentTests;
            subpassDepedency.srcAccessMask = vk::AccessFlagBits::eNone;  
            subpassDepedency.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;

            return subpassDepedency;
        }

        vk::RenderPass createRendepass(
            bool inHasDepth,
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
            subpass.colorAttachmentCount    = static_cast<uint32_t>(colorAttachmentRefs.size());
            subpass.pColorAttachments       = colorAttachmentRefs.data();

            vk::AttachmentReference depthAttachmentRef;

            if (inHasDepth)
            {
                subpassDependecies.push_back(createDepthSubpassDepedency());

                depthAttachmentRef = createDepthAttachmentRef();

                subpass.pDepthStencilAttachment = &depthAttachmentRef;
            }
    
            vk::RenderPassCreateInfo renderPassInfo {};
            renderPassInfo.flags           = vk::RenderPassCreateFlags();
            renderPassInfo.attachmentCount = static_cast<uint32_t>(inAttachments.size());
            renderPassInfo.pAttachments    = inAttachments.data();
            renderPassInfo.dependencyCount = static_cast<uint32_t>(subpassDependecies.size());
            renderPassInfo.pDependencies   = subpassDependecies.data();
            renderPassInfo.subpassCount    = 1;
            renderPassInfo.pSubpasses      = &subpass;
    
            return inLogicalDevice.createRenderPass(renderPassInfo);
        }

        Instance::Instance(const CreateInfo& inCreateInfo)
            : m_canOverwrite(inCreateInfo.canOverwrite),
            m_hasVertices(inCreateInfo.hasVertices),
            m_hasDepth(inCreateInfo.hasDepth),
            m_logicalDevice(inCreateInfo.logicalDevice),
            m_vertexShaderPath(inCreateInfo.vertexShaderPath),
            m_fragmentShaderPath(inCreateInfo.fragmentShaderPath),
            m_bindingDescription(inCreateInfo.bindingDescription),
            m_attributeDescriptions(inCreateInfo.attributeDescriptions),
            m_swapChainExtent(inCreateInfo.swapChainExtent),
            m_swapChainImageFormat(inCreateInfo.swapChainImageFormat),
            m_depthFormat(inCreateInfo.depthFormat),
            m_descriptorSetLayouts(inCreateInfo.descriptorSetLayouts)
        {
            init();
        }

        Instance::~Instance()
        {
            m_logicalDevice.waitIdle();

            m_logicalDevice.destroyPipeline(instance);
            m_logicalDevice.destroyPipelineLayout(layout);
            m_logicalDevice.destroyRenderPass(renderPass);
        }

        void Instance::init()
        {
            vk::ShaderModule vertexShaderModule;
            Shader::initModule(
                vertexShaderModule,
                m_vertexShaderPath,
                m_logicalDevice
            );

            vk::ShaderModule fragmentShaderModule;
            Shader::initModule(
                fragmentShaderModule,
                m_fragmentShaderPath,
                m_logicalDevice
            );

            vk::GraphicsPipelineCreateInfo pipelineInfo {};
            pipelineInfo.flags = vk::PipelineCreateFlags();

            // Vertex Input
            vk::PipelineVertexInputStateCreateInfo vertexInputState = createVertexInputState(
                m_hasVertices,
                m_bindingDescription,
                m_attributeDescriptions
            );
            pipelineInfo.pVertexInputState = &vertexInputState;

            // Input Assembly
            vk::PipelineInputAssemblyStateCreateInfo inputAsstembyState = createInputAssemblyState();
            pipelineInfo.pInputAssemblyState = &inputAsstembyState;

            // Viewport
            Vec<2, std::uint32_t> size(m_swapChainExtent.width, m_swapChainExtent.height);

            vk::Viewport viewport = createViewport(size);
            vk::Rect2D scissor = createScissor(size);

            vk::PipelineViewportStateCreateInfo viewportState = createViewportState(
                viewport,
                scissor
            );
            pipelineInfo.pViewportState = &viewportState;

            // Dynamic State
            std::vector<vk::DynamicState> dynamicStates = {
                vk::DynamicState::eViewport,
                vk::DynamicState::eScissor
            };

            vk::PipelineDynamicStateCreateInfo dynamicState = createDynamicState(dynamicStates);

            pipelineInfo.pDynamicState = &dynamicState;

            // Shader Stage
            std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = {
                createVertexShader(
                    vertexShaderModule
                ),
                createFragmentShader(
                    fragmentShaderModule
                )
            };
            pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
            pipelineInfo.pStages    = shaderStages.data();

            // Rasterization
            vk::PipelineRasterizationStateCreateInfo rasterizaterizationState = createRasterizationState();
            pipelineInfo.pRasterizationState = &rasterizaterizationState;

            // Sampling
            vk::PipelineMultisampleStateCreateInfo multisampleState = createMulitsampleState();
            pipelineInfo.pMultisampleState = &multisampleState;

            // Color Blending
            vk::PipelineColorBlendAttachmentState colorBlendAttachmentState {};
            colorBlendAttachmentState.colorWriteMask      = vk::ColorComponentFlagBits::eR |
                                                            vk::ColorComponentFlagBits::eG |
                                                            vk::ColorComponentFlagBits::eB |
                                                            vk::ColorComponentFlagBits::eA;
            colorBlendAttachmentState.blendEnable         = VK_TRUE;
            colorBlendAttachmentState.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
            colorBlendAttachmentState.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
            colorBlendAttachmentState.colorBlendOp        = vk::BlendOp::eAdd;
            colorBlendAttachmentState.srcAlphaBlendFactor = vk::BlendFactor::eSrcAlpha;
            colorBlendAttachmentState.dstAlphaBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
            colorBlendAttachmentState.alphaBlendOp        = vk::BlendOp::eSubtract;

            vk::PipelineColorBlendStateCreateInfo colorBlendState = createColorBlendState(colorBlendAttachmentState);
            pipelineInfo.pColorBlendState = &colorBlendState;

            // Depthning
            vk::PipelineDepthStencilStateCreateInfo depthStencilState;

            if (m_hasDepth)
            {
                depthStencilState = createDepthStencilState();
            }

            pipelineInfo.pDepthStencilState = &depthStencilState;

            layout = createLayout(
                m_descriptorSetLayouts,
                m_logicalDevice
            );

            std::vector<vk::AttachmentDescription> attachments;

            attachments.push_back(
                createColorAttachment(
                    m_swapChainImageFormat,
                    m_canOverwrite ? vk::AttachmentLoadOp::eLoad : vk::AttachmentLoadOp::eDontCare,
                    m_canOverwrite ? vk::ImageLayout::ePresentSrcKHR : vk::ImageLayout::eUndefined
                )
            );

            if (m_hasDepth)
            {
                attachments.push_back(
                    createDepthAttachment(m_depthFormat)
                );
            }

            renderPass = createRendepass(
                m_hasDepth,
                attachments,
                m_logicalDevice
            ); 

            pipelineInfo.layout             = layout;
            pipelineInfo.renderPass         = renderPass;
            pipelineInfo.subpass            = 0;
            pipelineInfo.basePipelineHandle = nullptr;

            instance = m_logicalDevice.createGraphicsPipeline(
                nullptr,
                pipelineInfo
            ).value;

            m_logicalDevice.destroyShaderModule(vertexShaderModule);
            m_logicalDevice.destroyShaderModule(fragmentShaderModule);
        }
    }
}