#include "Chicane/Runtime/Renderer/Vulkan/GraphicsPipeline/Instance.hpp"

#include "Chicane/Runtime/Renderer/Vulkan/GraphicsPipeline.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Shader.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Vertex.hpp"

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

                vk::VertexInputBindingDescription bindingDescription                   = Vertex::getBindingDescription();
                std::vector<vk::VertexInputAttributeDescription> attributeDescriptions = Vertex::getAttributeDescriptions();

                if (inCreateInfo.bHasVertices)
                {
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
                std::vector<vk::DynamicState> dynamicStates     = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
                vk::PipelineDynamicStateCreateInfo dynamicState = createDynamicState(dynamicStates);

                // Shader Stage
                std::vector<vk::PipelineShaderStageCreateInfo> shaders = {};
                for (const Shader::StageCreateInfo& shader : inCreateInfo.shaders)
                {
                    shaders.push_back(
                        Shader::initShaderStage(
                            m_logicalDevice,
                            shader
                        )
                    );
                }

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

                // Attachment
                std::vector<vk::AttachmentDescription> attachments = {};
                for (const Attachment& attachment : inCreateInfo.attachments)
                {
                    if (attachment.type == Attachment::Type::Color)
                    {
                        attachments.push_back(createColorAttachment(attachment));
                    }

                    if (attachment.type == Attachment::Type::Depth)
                    {
                        attachments.push_back(createDepthAttachment(attachment));
                    }
                }

                vk::GraphicsPipelineCreateInfo pipelineInfo {};
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
                pipelineInfo.renderPass          = createRendepass(
                    attachments,
                    m_logicalDevice,
                    std::find_if(
                        inCreateInfo.attachments.begin(),
                        inCreateInfo.attachments.end(),
                        [](const auto& inAttachment)
                        {
                            return inAttachment.type == Attachment::Type::Color;
                        }
                    ) != inCreateInfo.attachments.end(),
                    std::find_if(
                        inCreateInfo.attachments.begin(),
                        inCreateInfo.attachments.end(),
                        [](const auto& inAttachment)
                        {
                            return inAttachment.type == Attachment::Type::Depth;
                        }
                    ) != inCreateInfo.attachments.end()
                );
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

            void Instance::bind(vk::CommandBuffer& inCommandBuffer)
            {
                inCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, instance);
            }

            void Instance::bindDescriptorSet(
                vk::CommandBuffer& inCommandBuffer,
                std::uint32_t inIndex,
                vk::DescriptorSet inDescriptorSet
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
}