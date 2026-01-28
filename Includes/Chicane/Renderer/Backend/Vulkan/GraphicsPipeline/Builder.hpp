#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/GraphicsPipeline.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Shader.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanGraphicsPipelineBuilder
        {
        public:
            VulkanGraphicsPipelineBuilder()
                : m_vertexBindings({}),
                  m_vertexAttributes({}),
                  m_inputAssemby(vk::PipelineInputAssemblyStateCreateInfo()),
                  m_viewports({}),
                  m_scissors({}),
                  m_dynamicStates({}),
                  m_shaders({}),
                  m_multisample(vk::PipelineMultisampleStateCreateInfo()),
                  m_colorBlendings({}),
                  m_depthStencil(vk::PipelineDepthStencilStateCreateInfo()),
                  m_attachments({}),
                  m_subpassDepedencies({}),
                  m_subpasses({}),
                  m_descriptorSetLayouts({}),
                  m_pushConstants({})
            {}

        public:
            VulkanGraphicsPipelineBuilder addVertexBinding(vk::VertexInputBindingDescription inValue)
            {
                m_vertexBindings.push_back(inValue);

                return *this;
            }

            VulkanGraphicsPipelineBuilder addVertexAttributes(std::vector<vk::VertexInputAttributeDescription> inValues)
            {
                m_vertexAttributes.insert(m_vertexAttributes.end(), inValues.begin(), inValues.end());

                return *this;
            }

            VulkanGraphicsPipelineBuilder addVertexAttribute(vk::VertexInputAttributeDescription inValue)
            {
                m_vertexAttributes.push_back(inValue);

                return *this;
            }

            VulkanGraphicsPipelineBuilder setInputAssembly(vk::PipelineInputAssemblyStateCreateInfo inValue)
            {
                m_inputAssemby = inValue;

                return *this;
            }

            VulkanGraphicsPipelineBuilder addViewport(vk::Viewport inValue)
            {
                m_viewports.push_back(inValue);

                return *this;
            }

            VulkanGraphicsPipelineBuilder addScissor(vk::Rect2D inValue)
            {
                m_scissors.push_back(inValue);

                return *this;
            }

            VulkanGraphicsPipelineBuilder addDynamicState(vk::DynamicState inValue)
            {
                m_dynamicStates.push_back(inValue);

                return *this;
            }

            VulkanGraphicsPipelineBuilder addShaderStage(
                const VulkanShaderStageCreateInfo& inValue, const vk::Device& inLogicalDevice
            )
            {
                m_shaders.push_back(VulkanShader::initShaderStage(inLogicalDevice, inValue));

                return *this;
            }

            VulkanGraphicsPipelineBuilder setRasterization(vk::PipelineRasterizationStateCreateInfo inValue)
            {
                m_rasterization = inValue;

                return *this;
            }

            VulkanGraphicsPipelineBuilder setMultisample(vk::PipelineMultisampleStateCreateInfo inValue)
            {
                m_multisample = inValue;

                return *this;
            }

            VulkanGraphicsPipelineBuilder addColorBlendingAttachment(vk::PipelineColorBlendAttachmentState inValue)
            {
                m_colorBlendings.push_back(inValue);

                return *this;
            }

            VulkanGraphicsPipelineBuilder setDepthStencil(vk::PipelineDepthStencilStateCreateInfo inValue)
            {
                m_depthStencil = inValue;

                return *this;
            }

            VulkanGraphicsPipelineBuilder addAttachment(vk::AttachmentDescription inValue)
            {
                m_attachments.push_back(inValue);

                return *this;
            }

            VulkanGraphicsPipelineBuilder addSubpassDependecy(vk::SubpassDependency inValue)
            {
                m_subpassDepedencies.push_back(inValue);

                return *this;
            }

            VulkanGraphicsPipelineBuilder addSubpass(vk::SubpassDescription inValue)
            {
                m_subpasses.push_back(inValue);

                return *this;
            }

            VulkanGraphicsPipelineBuilder addDescriptorSetLayout(vk::DescriptorSetLayout inValue)
            {
                m_descriptorSetLayouts.push_back(inValue);

                return *this;
            }

            VulkanGraphicsPipelineBuilder addPushConstant(vk::PushConstantRange inValue)
            {
                m_pushConstants.push_back(inValue);

                return *this;
            }

            void build(VulkanGraphicsPipeline& outGraphicsPipeline, const vk::Device& inLogicalDevice)
            {
                // Vertex Input
                vk::PipelineVertexInputStateCreateInfo vertexInput;
                vertexInput.flags                           = vk::PipelineVertexInputStateCreateFlags();
                vertexInput.vertexBindingDescriptionCount   = static_cast<std::uint32_t>(m_vertexBindings.size());
                vertexInput.pVertexBindingDescriptions      = m_vertexBindings.data();
                vertexInput.vertexAttributeDescriptionCount = static_cast<std::uint32_t>(m_vertexAttributes.size());
                vertexInput.pVertexAttributeDescriptions    = m_vertexAttributes.data();

                // Viewport
                vk::PipelineViewportStateCreateInfo viewport;
                viewport.flags         = vk::PipelineViewportStateCreateFlags();
                viewport.viewportCount = static_cast<std::uint32_t>(m_viewports.size());
                viewport.pViewports    = m_viewports.data();
                viewport.scissorCount  = static_cast<std::uint32_t>(m_scissors.size());
                viewport.pScissors     = m_scissors.data();

                // Dynamic State
                vk::PipelineDynamicStateCreateInfo dynamicState;
                dynamicState.flags             = vk::PipelineDynamicStateCreateFlags();
                dynamicState.dynamicStateCount = static_cast<std::uint32_t>(m_dynamicStates.size());
                dynamicState.pDynamicStates    = m_dynamicStates.data();

                // Color Blending
                vk::PipelineColorBlendStateCreateInfo colorBlend;
                colorBlend.flags             = vk::PipelineColorBlendStateCreateFlags();
                colorBlend.blendConstants[0] = 0.0f;
                colorBlend.blendConstants[1] = 0.0f;
                colorBlend.blendConstants[2] = 0.0f;
                colorBlend.blendConstants[3] = 0.0f;
                colorBlend.attachmentCount   = static_cast<std::uint32_t>(m_colorBlendings.size());
                colorBlend.pAttachments      = m_colorBlendings.data();

                vk::GraphicsPipelineCreateInfo createInfo;
                createInfo.flags               = vk::PipelineCreateFlags();
                createInfo.pVertexInputState   = &vertexInput;
                createInfo.pInputAssemblyState = &m_inputAssemby;
                createInfo.pViewportState      = &viewport;
                createInfo.pDynamicState       = &dynamicState;
                createInfo.stageCount          = static_cast<std::uint32_t>(m_shaders.size());
                createInfo.pStages             = m_shaders.data();
                createInfo.pRasterizationState = &m_rasterization;
                createInfo.pMultisampleState   = &m_multisample;
                createInfo.pColorBlendState    = &colorBlend;
                createInfo.pDepthStencilState  = &m_depthStencil;
                createInfo.layout =
                    VulkanGraphicsPipeline::createLayout(m_descriptorSetLayouts, m_pushConstants, inLogicalDevice);
                createInfo.renderPass = VulkanGraphicsPipeline::createRendepass(
                    m_attachments,
                    m_subpassDepedencies,
                    m_subpasses,
                    inLogicalDevice
                );
                createInfo.subpass            = 0;
                createInfo.basePipelineHandle = nullptr;

                outGraphicsPipeline.init(inLogicalDevice, createInfo);
            }

        private:
            // Vertex Input
            std::vector<vk::VertexInputBindingDescription>     m_vertexBindings;
            std::vector<vk::VertexInputAttributeDescription>   m_vertexAttributes;

            // Input Assembly
            vk::PipelineInputAssemblyStateCreateInfo           m_inputAssemby;

            // Viewport
            std::vector<vk::Viewport>                          m_viewports;
            std::vector<vk::Rect2D>                            m_scissors;

            // Dynamic State
            std::vector<vk::DynamicState>                      m_dynamicStates;

            // Shader Stage
            std::vector<vk::PipelineShaderStageCreateInfo>     m_shaders;

            // Rasterization
            vk::PipelineRasterizationStateCreateInfo           m_rasterization;

            // Sampling
            vk::PipelineMultisampleStateCreateInfo             m_multisample;

            // Color Blending
            std::vector<vk::PipelineColorBlendAttachmentState> m_colorBlendings;

            // Depthning
            vk::PipelineDepthStencilStateCreateInfo            m_depthStencil;

            // Renderpass
            std::vector<vk::AttachmentDescription>             m_attachments;
            std::vector<vk::SubpassDependency>                 m_subpassDepedencies;
            std::vector<vk::SubpassDescription>                m_subpasses;

            // Layout
            std::vector<vk::DescriptorSetLayout>               m_descriptorSetLayouts;
            std::vector<vk::PushConstantRange>                 m_pushConstants;
        };
    }
}