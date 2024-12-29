#include "Chicane/Renderer/GraphicsPipeline/Instance.hpp"

#include "Chicane/Renderer/GraphicsPipeline.hpp"
#include "Chicane/Renderer/Shader.hpp"
#include "Chicane/Renderer/Vertex.hpp"

namespace Chicane
{
    namespace GraphicsPipeline
    {
        Instance::Instance(const CreateInfo& inCreateInfo)
            : m_bHasVertices(inCreateInfo.bHasVertices),
            m_bHasDepth(inCreateInfo.bHasDepth),
            m_vertexShaderPath(inCreateInfo.vertexShaderPath),
            m_fragmentShaderPath(inCreateInfo.fragmentShaderPath),
            m_bindingDescription(inCreateInfo.bindingDescription),
            m_attributeDescriptions(inCreateInfo.attributeDescriptions),
            m_extent(inCreateInfo.extent),
            m_descriptorSetLayouts(inCreateInfo.descriptorSetLayouts),
            m_pushConstantRanges(inCreateInfo.pushConstantRanges),
            m_logicalDevice(inCreateInfo.logicalDevice)
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
                m_bHasVertices,
                m_bindingDescription,
                m_attributeDescriptions
            );
            pipelineInfo.pVertexInputState = &vertexInputState;

            // Input Assembly
            vk::PipelineInputAssemblyStateCreateInfo inputAsstembyState = createInputAssemblyState();
            pipelineInfo.pInputAssemblyState = &inputAsstembyState;

            // Viewport
            Vec<2, std::uint32_t> size(m_extent.width, m_extent.height);

            vk::Viewport viewport = createViewport(size);
            vk::Rect2D scissor    = createScissor(size);

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
                createVertexShader(vertexShaderModule),
                createFragmentShader(fragmentShaderModule)
            };
            pipelineInfo.stageCount = static_cast<std::uint32_t>(shaderStages.size());
            pipelineInfo.pStages    = shaderStages.data();

            // Rasterization
            vk::PipelineRasterizationStateCreateInfo rasterizaterizationState = createRasterizationState(
                inCreateInfo.polygonMode
            );
            pipelineInfo.pRasterizationState = &rasterizaterizationState;

            // Sampling
            vk::PipelineMultisampleStateCreateInfo multisampleState = createMulitsampleState();
            pipelineInfo.pMultisampleState = &multisampleState;

            // Color Blending
            vk::PipelineColorBlendAttachmentState colorBlendAttachmentState = createBlendAttachmentState();
            colorBlendAttachmentState.blendEnable = inCreateInfo.bHasBlending ? VK_TRUE : VK_FALSE;

            vk::PipelineColorBlendStateCreateInfo colorBlendState = createColorBlendState();
            colorBlendState.attachmentCount = 1;
            colorBlendState.pAttachments    = &colorBlendAttachmentState;
            pipelineInfo.pColorBlendState = &colorBlendState;

            // Depthning
            vk::PipelineDepthStencilStateCreateInfo depthStencilState = createDepthStencilState();
            depthStencilState.depthTestEnable  = inCreateInfo.bHasDepth ? VK_TRUE : VK_FALSE;
            depthStencilState.depthWriteEnable = inCreateInfo.bHasDepth ? VK_TRUE : VK_FALSE;
            pipelineInfo.pDepthStencilState = &depthStencilState;

            layout = createLayout(
                m_descriptorSetLayouts,
                m_pushConstantRanges,
                m_logicalDevice
            );

            std::vector<vk::AttachmentDescription> attachments;

            attachments.push_back(
                createColorAttachment(inCreateInfo.colorAttachment)
            );

            if (m_bHasDepth)
            {
                attachments.push_back(
                    createDepthAttachment(inCreateInfo.depthAttachment)
                );
            }

            renderPass = createRendepass(
                m_bHasDepth,
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

        Instance::~Instance()
        {
            m_logicalDevice.waitIdle();

            m_logicalDevice.destroyPipeline(instance);
            m_logicalDevice.destroyPipelineLayout(layout);
            m_logicalDevice.destroyRenderPass(renderPass);
        }
    }
}