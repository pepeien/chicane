#include "GraphicsPipeline.hpp"

namespace Engine
{
    namespace GraphicsPipeline
    {
        vk::Viewport createViewport(const vk::Extent2D& inExtent)
        {
            vk::Viewport viewport = {};
            viewport.x        = 0;
            viewport.y        = 0;
            viewport.width    = static_cast<float>(inExtent.width);
            viewport.height   = static_cast<float>(inExtent.height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;

            return viewport;
        }

        vk::Rect2D createScissor(const vk::Extent2D& inExtent)
        {
            vk::Rect2D scissor = {};
            scissor.offset.x = 0;
            scissor.offset.y = 0;
            scissor.extent   = inExtent;

            return scissor;
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
            m_logicalDevice.destroyPipeline(instance);
            m_logicalDevice.destroyPipelineLayout(layout);
            m_logicalDevice.destroyRenderPass(renderPass);
        }
       
        vk::PipelineShaderStageCreateInfo Instance::createVertexShader(
            const vk::ShaderModule& inShaderModule
        )
        {
            vk::PipelineShaderStageCreateInfo vertexShaderInfo = {};
            vertexShaderInfo.flags  = vk::PipelineShaderStageCreateFlags();
            vertexShaderInfo.stage  = vk::ShaderStageFlagBits::eVertex;
            vertexShaderInfo.module = inShaderModule;
            vertexShaderInfo.pName  = "main";

            return vertexShaderInfo;
        }
    
        vk::PipelineShaderStageCreateInfo Instance::createFragmentShader(
            const vk::ShaderModule& inShaderModule
        )
        {
            vk::PipelineShaderStageCreateInfo fragmentShaderInfo = {};
            fragmentShaderInfo.flags  = vk::PipelineShaderStageCreateFlags();
            fragmentShaderInfo.stage  = vk::ShaderStageFlagBits::eFragment;
            fragmentShaderInfo.module = inShaderModule;
            fragmentShaderInfo.pName  = "main";

            return fragmentShaderInfo;
        }

        vk::PipelineVertexInputStateCreateInfo Instance::createVertexInputState()
        {
            vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
            vertexInputInfo.flags                           = vk::PipelineVertexInputStateCreateFlags();
            vertexInputInfo.vertexBindingDescriptionCount   = m_hasVertices ? 1                                                     : 0;
            vertexInputInfo.pVertexBindingDescriptions      = m_hasVertices ? &m_bindingDescription                                 : nullptr;
            vertexInputInfo.vertexAttributeDescriptionCount = m_hasVertices ? static_cast<uint32_t>(m_attributeDescriptions.size()) : 0;
            vertexInputInfo.pVertexAttributeDescriptions    = m_hasVertices ? m_attributeDescriptions.data()                        : nullptr;

            return vertexInputInfo;
        }
        
        vk::PipelineInputAssemblyStateCreateInfo Instance::createInputAssemblyState()
        {
            vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
            inputAssemblyInfo.flags    = vk::PipelineInputAssemblyStateCreateFlags();
            inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;

            return inputAssemblyInfo;
        }
    
        vk::PipelineViewportStateCreateInfo Instance::createViewportState(
            const vk::Viewport& inViewport,
            const vk::Rect2D& inScissor
        )
        {
            vk::PipelineViewportStateCreateInfo viewportState = {};
            viewportState.flags         = vk::PipelineViewportStateCreateFlags();
            viewportState.viewportCount = 1;
            viewportState.pViewports    = &inViewport;
            viewportState.scissorCount  = 1;
            viewportState.pScissors     = &inScissor;

            return viewportState;
        }

        vk::PipelineDynamicStateCreateInfo Instance::createDynamicState()
        {
            std::vector<vk::DynamicState> dynamicStates = {
                vk::DynamicState::eViewport,
                vk::DynamicState::eScissor
            };

            vk::PipelineDynamicStateCreateInfo dynamicState = {};
            dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
            dynamicState.pDynamicStates    = dynamicStates.data();

            return dynamicState;
        }
    
        vk::PipelineRasterizationStateCreateInfo Instance::createRasterizationState()
        {
            vk::PipelineRasterizationStateCreateInfo rasterizationState = {};
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

        vk::PipelineMultisampleStateCreateInfo Instance::createMulitsampleState()
        {
            vk::PipelineMultisampleStateCreateInfo multisampleState = {};
            multisampleState.flags                = vk::PipelineMultisampleStateCreateFlags();
            multisampleState.sampleShadingEnable  = VK_FALSE;
            multisampleState.rasterizationSamples = vk::SampleCountFlagBits::e1;

            return multisampleState;
        }
    
        vk::PipelineColorBlendStateCreateInfo Instance::createColorBlendState(
            const vk::PipelineColorBlendAttachmentState& colorBlendAttachmentState
        )
        {
            vk::PipelineColorBlendStateCreateInfo colorBlendState = {};
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

        vk::PipelineDepthStencilStateCreateInfo Instance::createDepthStencilState()
        {
            vk::PipelineDepthStencilStateCreateInfo depthStencilCreateInfo = {};
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
    
        vk::PipelineLayout Instance::createLayout()
        {
            vk::PipelineLayoutCreateInfo layoutCreateInfo = {};
            layoutCreateInfo.flags          = vk::PipelineLayoutCreateFlags();
            layoutCreateInfo.setLayoutCount = m_descriptorSetLayouts.size();
            layoutCreateInfo.pSetLayouts    = m_descriptorSetLayouts.data();

            return m_logicalDevice.createPipelineLayout(layoutCreateInfo);
        }

        vk::AttachmentDescription Instance::createColorAttachment()
        {
            vk::AttachmentDescription attachmentDescription = {};
            attachmentDescription.flags          = vk::AttachmentDescriptionFlags();
            attachmentDescription.format         = m_swapChainImageFormat;
            attachmentDescription.samples        = vk::SampleCountFlagBits::e1;
            attachmentDescription.loadOp         = m_canOverwrite ? vk::AttachmentLoadOp::eLoad : vk::AttachmentLoadOp::eDontCare;
            attachmentDescription.storeOp        = vk::AttachmentStoreOp::eStore;
            attachmentDescription.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
            attachmentDescription.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
            attachmentDescription.initialLayout  = m_canOverwrite ? vk::ImageLayout::ePresentSrcKHR : vk::ImageLayout::eUndefined;
            attachmentDescription.finalLayout    = vk::ImageLayout::ePresentSrcKHR;

            return attachmentDescription;
        }

        vk::AttachmentReference Instance::createColorAttachmentRef()
        {
            vk::AttachmentReference attachmentRef = {};
            attachmentRef.attachment = 0;
            attachmentRef.layout     = vk::ImageLayout::eColorAttachmentOptimal;

            return attachmentRef;
        }

        vk::SubpassDependency Instance::createColorSubpassDepedency()
        {
            vk::SubpassDependency subpassDepedency = {};
            subpassDepedency.srcSubpass    = VK_SUBPASS_EXTERNAL;
            subpassDepedency.dstSubpass    = 0;
            subpassDepedency.srcStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            subpassDepedency.srcAccessMask = vk::AccessFlagBits::eNone;
            subpassDepedency.dstStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            subpassDepedency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

            return subpassDepedency;
        }

        vk::AttachmentDescription Instance::createDepthAttachment()
        {
            vk::AttachmentDescription attachmentDescription = {};
            attachmentDescription.flags          = vk::AttachmentDescriptionFlags();
            attachmentDescription.format         = m_depthFormat;
            attachmentDescription.samples        = vk::SampleCountFlagBits::e1;
            attachmentDescription.loadOp         = vk::AttachmentLoadOp::eClear;
            attachmentDescription.storeOp        = vk::AttachmentStoreOp::eDontCare;
            attachmentDescription.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
            attachmentDescription.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
            attachmentDescription.initialLayout  = vk::ImageLayout::eUndefined;
            attachmentDescription.finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    
            return attachmentDescription;
        }

        vk::AttachmentReference Instance::createDepthAttachmentRef()
        {
            vk::AttachmentReference attachmentRef = {};
            attachmentRef.attachment = 1;
            attachmentRef.layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal;

            return attachmentRef;
        }

        vk::SubpassDependency Instance::createDepthSubpassDepedency()
        {
            vk::SubpassDependency subpassDepedency = {};
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

        vk::RenderPass Instance::createRendepass()
        {
            std::vector<vk::AttachmentDescription> attachments;
            attachments.push_back(createColorAttachment());

            vk::AttachmentReference depthAttachmentRef;

            std::vector<vk::AttachmentReference> colorAttachmentRefs;
            colorAttachmentRefs.push_back(createColorAttachmentRef());

            std::vector<vk::SubpassDependency> subpassDependecies;   
            subpassDependecies.push_back(createColorSubpassDepedency());

            vk::SubpassDescription subpass = {};
            subpass.flags                   = vk::SubpassDescriptionFlags();
            subpass.pipelineBindPoint       = vk::PipelineBindPoint::eGraphics;
            subpass.colorAttachmentCount    = colorAttachmentRefs.size();
            subpass.pColorAttachments       = colorAttachmentRefs.data();

            if (m_hasDepth)
            {
                attachments.push_back(         createDepthAttachment());
                subpassDependecies.push_back(  createDepthSubpassDepedency());

                depthAttachmentRef = createDepthAttachmentRef();

                subpass.pDepthStencilAttachment = &depthAttachmentRef;
            }
    
            vk::RenderPassCreateInfo renderPassInfo = {};
            renderPassInfo.flags           = vk::RenderPassCreateFlags();
            renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            renderPassInfo.pAttachments    = attachments.data();
            renderPassInfo.dependencyCount = static_cast<uint32_t>(subpassDependecies.size());
            renderPassInfo.pDependencies   = subpassDependecies.data();
            renderPassInfo.subpassCount    = 1;
            renderPassInfo.pSubpasses      = &subpass;
    
            return m_logicalDevice.createRenderPass(renderPassInfo);
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

            vk::GraphicsPipelineCreateInfo pipelineInfo = {};
            pipelineInfo.flags = vk::PipelineCreateFlags();

            // Vertex Input
            vk::PipelineVertexInputStateCreateInfo vertexInputState = createVertexInputState();
            pipelineInfo.pVertexInputState = &vertexInputState;

            // Input Assembly
            vk::PipelineInputAssemblyStateCreateInfo inputAsstembyState = createInputAssemblyState();
            pipelineInfo.pInputAssemblyState = &inputAsstembyState;

            // Viewport
            vk::Viewport viewport = createViewport(m_swapChainExtent);
            vk::Rect2D scissor = createScissor(m_swapChainExtent);

            vk::PipelineViewportStateCreateInfo viewportState = createViewportState(
                viewport,
                scissor
            );
            pipelineInfo.pViewportState = &viewportState;

            // Dynamic State
            vk::PipelineDynamicStateCreateInfo dynamicState = createDynamicState();
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
            vk::PipelineColorBlendAttachmentState colorBlendAttachmentState = {};
            colorBlendAttachmentState.colorWriteMask = vk::ColorComponentFlagBits::eR |
                                                       vk::ColorComponentFlagBits::eG |
                                                       vk::ColorComponentFlagBits::eB |
                                                       vk::ColorComponentFlagBits::eA;
            colorBlendAttachmentState.blendEnable = VK_FALSE;

            vk::PipelineColorBlendStateCreateInfo colorBlendState = createColorBlendState(colorBlendAttachmentState);
            pipelineInfo.pColorBlendState = &colorBlendState;

            // Depthning
            if (m_hasDepth)
            {
                vk::PipelineDepthStencilStateCreateInfo depthStencilState = createDepthStencilState();
                pipelineInfo.pDepthStencilState = &depthStencilState;
            }

            layout     = createLayout();
            renderPass = createRendepass();

            pipelineInfo.layout              = layout;
            pipelineInfo.renderPass          = renderPass;
            pipelineInfo.subpass             = 0;
            pipelineInfo.basePipelineHandle  = nullptr;

            instance = m_logicalDevice.createGraphicsPipeline(
                nullptr,
                pipelineInfo
            ).value;

            m_logicalDevice.destroyShaderModule(vertexShaderModule);
            m_logicalDevice.destroyShaderModule(fragmentShaderModule);
        }
    }
}