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

        enum class Type
        {
            STANDARD,
            SKY
        };

        class Instance
        {
        public:
            Instance(const CreateInfo& inCreateInfo);
            ~Instance();

        public:
            vk::PipelineLayout layout;
            vk::RenderPass renderPass;
            vk::Pipeline instance;

        private:
            vk::PipelineVertexInputStateCreateInfo createVertexInputState(
                const vk::VertexInputBindingDescription& inBindingDescription,
                const std::vector<vk::VertexInputAttributeDescription>& inAttributeDescriptions
            );
            vk::PipelineInputAssemblyStateCreateInfo createInputAssemblyState();
            vk::PipelineShaderStageCreateInfo createVertexShader(
                const vk::ShaderModule& inShaderModule
            );
            vk::PipelineViewportStateCreateInfo createViewport(
                const vk::Viewport& inViewport,
                const vk::Rect2D& inScissor
            );
            vk::PipelineRasterizationStateCreateInfo createRasterizerState();
            vk::PipelineShaderStageCreateInfo createFragmentShader(
                const vk::ShaderModule& inShaderModule
            );
            vk::PipelineDepthStencilStateCreateInfo createDepthStencil();
            vk::PipelineMultisampleStateCreateInfo createMulitsampleState();
            vk::PipelineColorBlendStateCreateInfo createColorBlendState(
                const vk::PipelineColorBlendAttachmentState& inColorBlendAttachmentState
            );
            vk::PipelineLayout createLayout();
            vk::AttachmentDescription createColorAttachment();
            vk::AttachmentReference createColorAttachmentRef();
            vk::SubpassDependency createColorSubpassDepedency();
            vk::AttachmentDescription createDepthAttachment();
            vk::AttachmentReference createDepthAttachmentRef();
            vk::SubpassDependency createDepthSubpassDepedency();
            vk::RenderPass createRendepass();

            void init();

        private:
            vk::Device m_logicalDevice;
            std::string m_vertexShaderName;
            std::string m_fragmentShaderName;
            vk::Extent2D m_swapChainExtent;
            vk::Format m_swapChainImageFormat;
            vk::Format m_depthFormat;
            std::vector<vk::DescriptorSetLayout> m_descriptorSetLayouts;
        };
    }
}