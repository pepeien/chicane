#pragma once

#include "Base.hpp"

#include "Shader.hpp"
#include "Vertex.hpp"

namespace Engine
{
    namespace GraphicsPipeline
    {

        enum class Type
        {
            SKY,
            SCENE
        };

        struct CreateInfo
        {
            // Modifiers

            bool canOverwrite;
            bool hasVertices;
            bool hasDepth;

            // Devices

            vk::Device logicalDevice;

            // Vertex

            std::string vertexShaderPath;
            std::string fragmentShaderPath;
            vk::VertexInputBindingDescription bindingDescription;
            std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;

            // Viewport

            vk::Extent2D swapChainExtent;
            vk::Format swapChainImageFormat;
            vk::Format depthFormat; // Optional if `hasDepth` == `false`

            // Depth

            std::vector<vk::DescriptorSetLayout> descriptorSetLayouts; // Optional if `hasDepth` == `false`
        };

        vk::Viewport createViewport(const vk::Extent2D& inExtent);
        vk::Rect2D createScissor(const vk::Extent2D& inExtent);

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
            vk::PipelineShaderStageCreateInfo createVertexShader(
                const vk::ShaderModule& inShaderModule
            );
            vk::PipelineShaderStageCreateInfo createFragmentShader(
                const vk::ShaderModule& inShaderModule
            );
            vk::PipelineVertexInputStateCreateInfo createVertexInputState();
            vk::PipelineInputAssemblyStateCreateInfo createInputAssemblyState();
            vk::PipelineViewportStateCreateInfo createViewportState(
                const vk::Viewport& inViewport,
                const vk::Rect2D& inScissor
            );
            vk::PipelineDynamicStateCreateInfo createDynamicState();
            vk::PipelineRasterizationStateCreateInfo createRasterizationState();
            vk::PipelineMultisampleStateCreateInfo createMulitsampleState();
            vk::PipelineColorBlendStateCreateInfo createColorBlendState(
                const vk::PipelineColorBlendAttachmentState& inColorBlendAttachmentState
            );
            vk::PipelineDepthStencilStateCreateInfo createDepthStencilState();

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
            // Modifiers
            bool m_canOverwrite;
            bool m_hasVertices;
            bool m_hasDepth;

            // Devices
            vk::Device m_logicalDevice;

            // Vertex
            std::string m_vertexShaderPath;
            std::string m_fragmentShaderPath;
            vk::VertexInputBindingDescription m_bindingDescription;
            std::vector<vk::VertexInputAttributeDescription> m_attributeDescriptions;

            // Viewport
            vk::Extent2D m_swapChainExtent;
            vk::Format m_swapChainImageFormat;

            // Depth
            vk::Format m_depthFormat;

            // Layout
            std::vector<vk::DescriptorSetLayout> m_descriptorSetLayouts;
        };
    }
}