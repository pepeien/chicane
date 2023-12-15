#pragma once

#include "Base.hpp"

#include "Shader.hpp"
#include "Vertex/2D.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            namespace GraphicsPipeline
            {
                struct CreateInfo
                {
                    vk::Device logicalDevice;
                    std::string vertexShaderName;
                    std::string fragmentShaderName;
                    vk::Extent2D swapChainExtent;
                    vk::Format swapChainImageFormat;
                    vk::DescriptorSetLayout descriptorSetLayout;
                };

                struct Bundle
                {
                    vk::PipelineLayout layout;
                    vk::RenderPass renderPass;
                    vk::Pipeline instance;
                };

                vk::PipelineVertexInputStateCreateInfo createVertexInputState(
                    const vk::VertexInputBindingDescription& inBindingDescription,
                    const std::array<vk::VertexInputAttributeDescription, 2>& inAttributeDescriptions
                );
                vk::PipelineInputAssemblyStateCreateInfo createInputAssemblyState();
                vk::PipelineShaderStageCreateInfo createVertexShader(
                    const vk::ShaderModule& inShaderModule,
                    const CreateInfo& inCreateInfo
                );
                vk::PipelineViewportStateCreateInfo createViewport(
                    const CreateInfo& inCreateInfo,
                    const vk::Viewport& inViewport,
                    const vk::Rect2D& inScissor
                );
                vk::PipelineRasterizationStateCreateInfo createRasterizerState();
                vk::PipelineShaderStageCreateInfo createFragmentShader(
                    const vk::ShaderModule& inShaderModule,
                    const CreateInfo& inCreateInfo
                );
                vk::PipelineMultisampleStateCreateInfo createMulitsampleState();
                vk::PipelineColorBlendStateCreateInfo createColorBlendState(const vk::PipelineColorBlendAttachmentState& inColorBlendAttachmentState);
                vk::PipelineLayout createLayout(const CreateInfo& inCreateInfo);
                vk::RenderPass createRendepass(const CreateInfo& inCreateInfo);

                void init(Bundle& outGraphicsPipeline, const CreateInfo& inCreateInfo);
            }
        }
    }
}