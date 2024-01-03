#include "GraphicsPipeline.hpp"

namespace Chicane
{
    namespace GraphicsPipeline
    {
        Instance::Instance(const CreateInfo& inCreateInfo)
            : m_logicalDevice(inCreateInfo.logicalDevice),
            m_vertexShaderName(inCreateInfo.vertexShaderName),
            m_fragmentShaderName(inCreateInfo.fragmentShaderName),
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

        vk::PipelineVertexInputStateCreateInfo Instance::createVertexInputState(
            const vk::VertexInputBindingDescription& inBindingDescription,
            const std::vector<vk::VertexInputAttributeDescription>& inAttributeDescriptions
        )
        {
            vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
            vertexInputInfo.flags                           = vk::PipelineVertexInputStateCreateFlags();
            vertexInputInfo.vertexBindingDescriptionCount   = 1;
            vertexInputInfo.pVertexBindingDescriptions      = &inBindingDescription;
            vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(
                inAttributeDescriptions.size()
            );
            vertexInputInfo.pVertexAttributeDescriptions    = inAttributeDescriptions.data();

            return vertexInputInfo;
        }
        
        vk::PipelineInputAssemblyStateCreateInfo Instance::createInputAssemblyState()
        {
            vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
            inputAssemblyInfo.flags    = vk::PipelineInputAssemblyStateCreateFlags();
            inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;

            return inputAssemblyInfo;
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
    
        vk::PipelineViewportStateCreateInfo Instance::createViewport(
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
    
        vk::PipelineRasterizationStateCreateInfo Instance::createRasterizerState()
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

        vk::PipelineDepthStencilStateCreateInfo Instance::createDepthStencil()
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
            attachmentDescription.loadOp         = vk::AttachmentLoadOp::eClear;
            attachmentDescription.storeOp        = vk::AttachmentStoreOp::eStore;
            attachmentDescription.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
            attachmentDescription.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
            attachmentDescription.initialLayout  = vk::ImageLayout::eUndefined;
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
            attachmentDescription.storeOp        = vk::AttachmentStoreOp::eStore;
            attachmentDescription.stencilLoadOp  = vk::AttachmentLoadOp::eClear;
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
            attachments.push_back(createDepthAttachment());

            std::vector<vk::AttachmentReference> attachmentReferences;
            attachmentReferences.push_back(createColorAttachmentRef());
            attachmentReferences.push_back(createDepthAttachmentRef());

            std::vector<vk::SubpassDependency> subpassDependecies;   
            subpassDependecies.push_back(createColorSubpassDepedency());
            subpassDependecies.push_back(createDepthSubpassDepedency());

            vk::SubpassDescription subpass = {};
            subpass.flags                   = vk::SubpassDescriptionFlags();
            subpass.pipelineBindPoint       = vk::PipelineBindPoint::eGraphics;
            subpass.colorAttachmentCount    = 1;
            subpass.pColorAttachments       = &attachmentReferences[0];
            subpass.pDepthStencilAttachment = &attachmentReferences[1];
    
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
                m_vertexShaderName,
                m_logicalDevice
            );

            vk::ShaderModule fragmentShaderModule;
            Shader::initModule(
                fragmentShaderModule,
                m_fragmentShaderName,
                m_logicalDevice
            );

            vk::Viewport viewport = {};
            viewport.x        = 0;
            viewport.y        = 0;
            viewport.width    = static_cast<float>(m_swapChainExtent.width);
            viewport.height   = static_cast<float>(m_swapChainExtent.height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;

            vk::Rect2D scissor = {};
            scissor.offset.x = 0;
            scissor.offset.y = 0;
            scissor.extent   = m_swapChainExtent;

            vk::PipelineColorBlendAttachmentState colorBlendAttachmentState = {};
            colorBlendAttachmentState.colorWriteMask = vk::ColorComponentFlagBits::eR |
                                                       vk::ColorComponentFlagBits::eG |
                                                       vk::ColorComponentFlagBits::eB |
                                                       vk::ColorComponentFlagBits::eA;
            colorBlendAttachmentState.blendEnable = VK_FALSE;

            auto bindingDescription    = Vertex::getBindingDescription();
            auto attributeDescriptions = Vertex::getAttributeDescriptions();

            vk::PipelineVertexInputStateCreateInfo vertexInputState = createVertexInputState(
                bindingDescription,
                attributeDescriptions
            );
            vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState = createInputAssemblyState();
            vk::PipelineShaderStageCreateInfo vertexShaderCreateInfo = createVertexShader(
                vertexShaderModule
            );
            vk::PipelineViewportStateCreateInfo viewportState = createViewport(
                viewport,
                scissor
            );
            vk::PipelineRasterizationStateCreateInfo rasterizationState = createRasterizerState();
            vk::PipelineShaderStageCreateInfo fragmentShaderCreateInfo = createFragmentShader(
                fragmentShaderModule
            );
            vk::PipelineDepthStencilStateCreateInfo depthStencilState = createDepthStencil();
            vk::PipelineMultisampleStateCreateInfo multisampleState = createMulitsampleState();
            vk::PipelineColorBlendStateCreateInfo colorBlendState = createColorBlendState(
                colorBlendAttachmentState
            );

            layout     = createLayout();
            renderPass = createRendepass();

            std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
            shaderStages.push_back(vertexShaderCreateInfo);
            shaderStages.push_back(fragmentShaderCreateInfo);
 
            vk::GraphicsPipelineCreateInfo pipelineInfo = {};
            pipelineInfo.flags               = vk::PipelineCreateFlags();
            pipelineInfo.pVertexInputState   = &vertexInputState;
            pipelineInfo.pInputAssemblyState = &inputAssemblyState;
            pipelineInfo.pViewportState      = &viewportState;
            pipelineInfo.pRasterizationState = &rasterizationState;
            pipelineInfo.stageCount          = static_cast<uint32_t>(shaderStages.size());
            pipelineInfo.pStages             = shaderStages.data();
            pipelineInfo.pMultisampleState   = &multisampleState;
            pipelineInfo.pColorBlendState    = &colorBlendState;
            pipelineInfo.pDepthStencilState  = &depthStencilState; 
            pipelineInfo.layout              = layout;
            pipelineInfo.renderPass          = renderPass;
            pipelineInfo.subpass             = 0;
            pipelineInfo.basePipelineHandle  = nullptr;

            instance   = m_logicalDevice.createGraphicsPipeline(nullptr, pipelineInfo).value;

            m_logicalDevice.destroyShaderModule(vertexShaderModule);
            m_logicalDevice.destroyShaderModule(fragmentShaderModule);
        }
    }
}