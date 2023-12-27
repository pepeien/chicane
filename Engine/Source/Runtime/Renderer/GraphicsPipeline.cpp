#include "GraphicsPipeline.hpp"

namespace Chicane
{
    namespace GraphicsPipeline
    {
        vk::PipelineVertexInputStateCreateInfo createVertexInputState(
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
        
        vk::PipelineInputAssemblyStateCreateInfo createInputAssemblyState()
        {
            vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
            inputAssemblyInfo.flags    = vk::PipelineInputAssemblyStateCreateFlags();
            inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;
        
            return inputAssemblyInfo;
        }
        
        vk::PipelineShaderStageCreateInfo createVertexShader(
            const vk::ShaderModule& inShaderModule,
            const CreateInfo& inCreateInfo
        )
        {
            vk::PipelineShaderStageCreateInfo vertexShaderInfo = {};
            vertexShaderInfo.flags  = vk::PipelineShaderStageCreateFlags();
            vertexShaderInfo.stage  = vk::ShaderStageFlagBits::eVertex;
            vertexShaderInfo.module = inShaderModule;
            vertexShaderInfo.pName  = "main";
    
            return vertexShaderInfo;
        }
    
        vk::PipelineViewportStateCreateInfo createViewport(
            const CreateInfo& inCreateInfo,
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
    
        vk::PipelineRasterizationStateCreateInfo createRasterizerState()
        {
            vk::PipelineRasterizationStateCreateInfo rasterizationState = {};
            rasterizationState.flags                   = vk::PipelineRasterizationStateCreateFlags();
            rasterizationState.depthClampEnable        = VK_FALSE;
            rasterizationState.rasterizerDiscardEnable = VK_FALSE;
            rasterizationState.polygonMode             = vk::PolygonMode::eFill;
            rasterizationState.lineWidth               = 1.0f;
            rasterizationState.cullMode                = vk::CullModeFlagBits::eBack;
            rasterizationState.frontFace               = vk::FrontFace::eClockwise;
            rasterizationState.depthBiasEnable         = VK_FALSE;
    
            return rasterizationState;
        }
    
        vk::PipelineShaderStageCreateInfo createFragmentShader(
            const vk::ShaderModule& inShaderModule,
            const CreateInfo& inCreateInfo
        )
        {
            vk::PipelineShaderStageCreateInfo fragmentShaderInfo = {};
            fragmentShaderInfo.flags  = vk::PipelineShaderStageCreateFlags();
            fragmentShaderInfo.stage  = vk::ShaderStageFlagBits::eFragment;
            fragmentShaderInfo.module = inShaderModule;
            fragmentShaderInfo.pName  = "main";
    
            return fragmentShaderInfo;
        }

        vk::PipelineDepthStencilStateCreateInfo createDepthStencil()
        {
            vk::PipelineDepthStencilStateCreateInfo depthStencilCreateInfo = {};
            depthStencilCreateInfo.flags                 = vk::PipelineDepthStencilStateCreateFlags();
            depthStencilCreateInfo.depthTestEnable       = VK_FALSE;
            depthStencilCreateInfo.depthWriteEnable      = VK_FALSE;
            depthStencilCreateInfo.depthCompareOp        = vk::CompareOp::eEqual;
            depthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
            depthStencilCreateInfo.stencilTestEnable     = VK_FALSE;
            depthStencilCreateInfo.minDepthBounds        = 0.0f;
            depthStencilCreateInfo.maxDepthBounds        = 1.0f;

            return depthStencilCreateInfo;
        }
    
        vk::PipelineMultisampleStateCreateInfo createMulitsampleState()
        {
            vk::PipelineMultisampleStateCreateInfo multisampleState = {};
            multisampleState.flags                = vk::PipelineMultisampleStateCreateFlags();
            multisampleState.sampleShadingEnable  = VK_FALSE;
            multisampleState.rasterizationSamples = vk::SampleCountFlagBits::e1;
    
            return multisampleState;
        }
    
        vk::PipelineColorBlendStateCreateInfo createColorBlendState(
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
    
        vk::PipelineLayout createLayout(const CreateInfo& inCreateInfo)
        {
            vk::PipelineLayoutCreateInfo layoutCreateInfo = {};
            layoutCreateInfo.flags          = vk::PipelineLayoutCreateFlags();
            layoutCreateInfo.setLayoutCount = inCreateInfo.descriptorSetLayouts.size();
            layoutCreateInfo.pSetLayouts    = inCreateInfo.descriptorSetLayouts.data();
    
            return inCreateInfo.logicalDevice.createPipelineLayout(layoutCreateInfo);
        }

        vk::AttachmentDescription createColorAttachment(const CreateInfo& inCreateInfo)
        {
            vk::AttachmentDescription attachmentDescription = {};
            attachmentDescription.flags          = vk::AttachmentDescriptionFlags();
            attachmentDescription.format         = inCreateInfo.swapChainImageFormat;
            attachmentDescription.samples        = vk::SampleCountFlagBits::e1;
            attachmentDescription.loadOp         = vk::AttachmentLoadOp::eClear;
            attachmentDescription.storeOp        = vk::AttachmentStoreOp::eStore;
            attachmentDescription.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
            attachmentDescription.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
            attachmentDescription.initialLayout  = vk::ImageLayout::eUndefined;
            attachmentDescription.finalLayout    = vk::ImageLayout::ePresentSrcKHR;
    
            return attachmentDescription;
        }

        vk::AttachmentReference createColorAttachmentRef()
        {
            vk::AttachmentReference attachmentRef = {};
            attachmentRef.attachment = 0;
            attachmentRef.layout     = vk::ImageLayout::eColorAttachmentOptimal;

            return attachmentRef;
        }

        vk::SubpassDependency createColorSubpassDepedency()
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

        vk::AttachmentDescription createDepthAttachment(const CreateInfo& inCreateInfo)
        {
            vk::AttachmentDescription attachmentDescription = {};
            attachmentDescription.flags          = vk::AttachmentDescriptionFlags();
            attachmentDescription.format         = inCreateInfo.depthFormat;
            attachmentDescription.samples        = vk::SampleCountFlagBits::e1;
            attachmentDescription.loadOp         = vk::AttachmentLoadOp::eClear;
            attachmentDescription.storeOp        = vk::AttachmentStoreOp::eStore;
            attachmentDescription.stencilLoadOp  = vk::AttachmentLoadOp::eClear;
            attachmentDescription.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
            attachmentDescription.initialLayout  = vk::ImageLayout::eUndefined;
            attachmentDescription.finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    
            return attachmentDescription;
        }

        vk::AttachmentReference createDepthAttachmentRef()
        {
            vk::AttachmentReference attachmentRef = {};
            attachmentRef.attachment = 1;
            attachmentRef.layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal;

            return attachmentRef;
        }

        vk::SubpassDependency createDepthSubpassDepedency()
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
    
        vk::RenderPass createRendepass(const CreateInfo& inCreateInfo)
        {
            std::vector<vk::AttachmentDescription> attachments;
            attachments.push_back(createColorAttachment(inCreateInfo));
            attachments.push_back(createDepthAttachment(inCreateInfo));

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
    
            return inCreateInfo.logicalDevice.createRenderPass(renderPassInfo);
        }
    
        void init(
            Bundle& outGraphicsPipeline,
            const CreateInfo& inCreateInfo
        )
        {
            vk::ShaderModule vertexShaderModule;
            Shader::initModule(
                vertexShaderModule,
                inCreateInfo.vertexShaderName,
                inCreateInfo.logicalDevice
            );
    
            vk::ShaderModule fragmentShaderModule;
            Shader::initModule(
                fragmentShaderModule,
                inCreateInfo.fragmentShaderName,
                inCreateInfo.logicalDevice
            );
    
            vk::Viewport viewport = {};
            viewport.x        = 0;
            viewport.y        = 0;
            viewport.width    = static_cast<float>(inCreateInfo.swapChainExtent.width);
            viewport.height   = static_cast<float>(inCreateInfo.swapChainExtent.height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
    
            vk::Rect2D scissor = {};
            scissor.offset.x = 0;
            scissor.offset.y = 0;
            scissor.extent   = inCreateInfo.swapChainExtent;
    
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
                vertexShaderModule,
                inCreateInfo
            );
            vk::PipelineViewportStateCreateInfo viewportState = createViewport(
                inCreateInfo,
                viewport,
                scissor
            );
            vk::PipelineRasterizationStateCreateInfo rasterizationState = createRasterizerState();
            vk::PipelineShaderStageCreateInfo fragmentShaderCreateInfo = createFragmentShader(
                fragmentShaderModule,
                inCreateInfo
            );
            vk::PipelineDepthStencilStateCreateInfo depthStencilState = createDepthStencil();
            vk::PipelineMultisampleStateCreateInfo multisampleState = createMulitsampleState();
            vk::PipelineColorBlendStateCreateInfo colorBlendState = createColorBlendState(
                colorBlendAttachmentState
            );
            vk::PipelineLayout layout = createLayout(inCreateInfo);
            vk::RenderPass renderPass = createRendepass(inCreateInfo);
    
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
    
            outGraphicsPipeline.layout     = layout;
            outGraphicsPipeline.renderPass = renderPass;
            outGraphicsPipeline.instance   = inCreateInfo
                                             .logicalDevice
                                             .createGraphicsPipeline(
                                                nullptr,
                                                pipelineInfo
                                             ).value;
    
            inCreateInfo.logicalDevice.destroyShaderModule(vertexShaderModule);
            inCreateInfo.logicalDevice.destroyShaderModule(fragmentShaderModule);
        }
    }
}