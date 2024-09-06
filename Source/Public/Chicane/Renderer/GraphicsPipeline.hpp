#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Renderer/Shader.hpp"
#include "Chicane/Renderer/Vertex.hpp"

namespace Chicane
{
    namespace GraphicsPipeline
    {
        vk::Viewport createViewport(
            const Vec<2, std::uint32_t>& inSize = Vec<2, std::uint32_t>(0),
            const Vec<2, float>& inPosition = Vec<2, float>(0.0f)
        );
        vk::Rect2D createScissor(const Vec<2, std::uint32_t>& inSize = Vec<2, std::uint32_t>(0));
        vk::PipelineShaderStageCreateInfo createVertexShader(
            const vk::ShaderModule& inShaderModule
        );
        vk::PipelineShaderStageCreateInfo createFragmentShader(
            const vk::ShaderModule& inShaderModule
        );
        vk::PipelineVertexInputStateCreateInfo createVertexInputState(
            bool isHasVertices,
            const vk::VertexInputBindingDescription& inBindingDescription,
            const std::vector<vk::VertexInputAttributeDescription>& inAttributeDescriptions
        );
        vk::PipelineInputAssemblyStateCreateInfo createInputAssemblyState();
        vk::PipelineViewportStateCreateInfo createViewportState(
            const vk::Viewport& inViewport,
            const vk::Rect2D& inScissor
        );
        vk::PipelineDynamicStateCreateInfo createDynamicState(const std::vector<vk::DynamicState>& inDynamicStates);
        vk::PipelineRasterizationStateCreateInfo createRasterizationState(vk::PolygonMode inPolygonMode);
        vk::PipelineMultisampleStateCreateInfo createMulitsampleState();
        vk::PipelineColorBlendAttachmentState createBlendAttachmentState();
        vk::PipelineColorBlendStateCreateInfo createColorBlendState();
        vk::PipelineDepthStencilStateCreateInfo createDepthStencilState();

        vk::PipelineLayout createLayout(
            const std::vector<vk::DescriptorSetLayout>& inDescriptorSetLayouts,
            const vk::Device& inLogicalDevice
        );
        vk::AttachmentDescription createColorAttachment(
            vk::Format inFormat,
            vk::AttachmentLoadOp inLoadOp,
            vk::ImageLayout inInitialLayout
        );
        vk::AttachmentReference createColorAttachmentRef();
        vk::SubpassDependency createColorSubpassDepedency();
        vk::AttachmentDescription createDepthAttachment(const vk::Format& inFormat);
        vk::AttachmentReference createDepthAttachmentRef();
        vk::SubpassDependency createDepthSubpassDepedency();
        vk::RenderPass createRendepass(
            bool inHasDepth,
            const std::vector<vk::AttachmentDescription>& inAttachments,
            const vk::Device& inLogicalDevice
        );

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

            vk::PolygonMode polygonMode;
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

            vk::PolygonMode m_polygonMode;
        };
    }
}