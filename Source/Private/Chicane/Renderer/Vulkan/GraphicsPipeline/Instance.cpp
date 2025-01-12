#include "Chicane/Renderer/Vulkan/GraphicsPipeline/Instance.hpp"

#include "Chicane/Renderer/Vulkan/GraphicsPipeline.hpp"
#include "Chicane/Renderer/Vulkan/Shader.hpp"
#include "Chicane/Renderer/Vulkan/Vertex.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace GraphicsPipeline
        {
            Instance::Instance(const CreateInfo& inCreateInfo)
                : m_logicalDevice(inCreateInfo.logicalDevice)
            {
                // Vertex Input
                vk::PipelineVertexInputStateCreateInfo vertexInputState = createVertexInputState();

                if (inCreateInfo.bHasVertices)
                {
                    vk::VertexInputBindingDescription bindingDescription                   = Vertex::getBindingDescription();
                    std::vector<vk::VertexInputAttributeDescription> attributeDescriptions = Vertex::getAttributeDescriptions();

                    vertexInputState.vertexBindingDescriptionCount   = 1;
                    vertexInputState.pVertexBindingDescriptions      = &bindingDescription;
                    vertexInputState.vertexAttributeDescriptionCount = static_cast<std::uint32_t>(attributeDescriptions.size());
                    vertexInputState.pVertexAttributeDescriptions    = attributeDescriptions.data();
                }

                // Input Assembly
                vk::PipelineInputAssemblyStateCreateInfo inputAsstembyState = createInputAssemblyState();

                // Viewport
                Vec<2, std::uint32_t> size(inCreateInfo.extent.width, inCreateInfo.extent.height);

                vk::PipelineViewportStateCreateInfo viewportState = createViewportState(
                    createViewport(size),
                    createScissor(size)
                );

                // Dynamic State
                vk::PipelineDynamicStateCreateInfo dynamicState = createDynamicState(
                    {
                        vk::DynamicState::eViewport,
                        vk::DynamicState::eScissor
                    }
                );

                // Shader Stage
                std::vector<vk::PipelineShaderStageCreateInfo> shaders {};
                for (const Shader::StageCreateInfo& shader : inCreateInfo.shaders)
                {
                    shaders.push_back(
                        Shader::initShaderStage(
                            m_logicalDevice,
                            shader
                        )
                    );
                }

                // Rasterization
                vk::PipelineRasterizationStateCreateInfo rasterizaterizationState = createRasterizationState(
                    inCreateInfo.polygonMode
                );

                // Sampling
                vk::PipelineMultisampleStateCreateInfo multisampleState = createMulitsampleState();

                // Color Blending
                vk::PipelineColorBlendAttachmentState colorBlendAttachmentState = createBlendAttachmentState();
                colorBlendAttachmentState.blendEnable = inCreateInfo.bHasBlending ? VK_TRUE : VK_FALSE;

                vk::PipelineColorBlendStateCreateInfo colorBlendState = createColorBlendState();
                colorBlendState.attachmentCount = 1;
                colorBlendState.pAttachments    = &colorBlendAttachmentState;

                // Depthning
                vk::PipelineDepthStencilStateCreateInfo depthStencilState = createDepthStencilState();
                depthStencilState.depthTestEnable  = VK_TRUE;
                depthStencilState.depthWriteEnable = inCreateInfo.bHasDepthWrite ? VK_TRUE : VK_FALSE;

                vk::GraphicsPipelineCreateInfo pipelineInfo {};
                pipelineInfo.flags               = vk::PipelineCreateFlags();
                pipelineInfo.pVertexInputState   = &vertexInputState;
                pipelineInfo.pInputAssemblyState = &inputAsstembyState;
                pipelineInfo.pViewportState      = &viewportState;
                pipelineInfo.pDynamicState       = &dynamicState;
                pipelineInfo.stageCount          = static_cast<std::uint32_t>(shaders.size());
                pipelineInfo.pStages             = shaders.data();
                pipelineInfo.pRasterizationState = &rasterizaterizationState;
                pipelineInfo.pMultisampleState   = &multisampleState;
                pipelineInfo.pColorBlendState    = &colorBlendState;
                pipelineInfo.pDepthStencilState  = &depthStencilState;
                pipelineInfo.layout              = createLayout(inCreateInfo.descriptorSetLayouts, m_logicalDevice);
                pipelineInfo.renderPass          = createRendepass(inCreateInfo.attachments, m_logicalDevice);
                pipelineInfo.subpass             = 0;
                pipelineInfo.basePipelineHandle  = nullptr;

                layout     = pipelineInfo.layout;
                renderPass = pipelineInfo.renderPass; 
                instance   = m_logicalDevice.createGraphicsPipeline(nullptr, pipelineInfo).value;

                for (vk::PipelineShaderStageCreateInfo& shader : shaders)
                {
                    m_logicalDevice.destroyShaderModule(shader.module);
                }
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
}