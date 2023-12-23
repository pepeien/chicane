#pragma once

#include "Base.hpp"

#include "Shader.hpp"
#include "Vertex.hpp"

namespace Chicane
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
            vk::Format depthFormat;
            std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
        };
    
        struct Bundle
        {
            vk::PipelineLayout layout;
            vk::RenderPass renderPass;
            vk::Pipeline instance;
        };
    
        vk::PipelineVertexInputStateCreateInfo createVertexInputState(
            const vk::VertexInputBindingDescription& inBindingDescription,
            const std::vector<vk::VertexInputAttributeDescription>& inAttributeDescriptions
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
        vk::PipelineDepthStencilStateCreateInfo createDepthStencil();
        vk::PipelineMultisampleStateCreateInfo createMulitsampleState();
        vk::PipelineColorBlendStateCreateInfo createColorBlendState(
            const vk::PipelineColorBlendAttachmentState& inColorBlendAttachmentState
        );
        vk::PipelineLayout createLayout(const CreateInfo& inCreateInfo);
        vk::AttachmentDescription createColorAttachment(const CreateInfo& inCreateInfo);
        vk::AttachmentReference createColorAttachmentRef();
        vk::AttachmentDescription createDepthAttachment(const CreateInfo& inCreateInfo);
        vk::AttachmentReference createDepthAttachmentRef();
        vk::RenderPass createRendepass(const CreateInfo& inCreateInfo);
    
        void init(Bundle& outGraphicsPipeline, const CreateInfo& inCreateInfo);
    }
}