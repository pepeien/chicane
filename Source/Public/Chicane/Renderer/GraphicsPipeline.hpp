#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Renderer/Shader.hpp"
#include "Chicane/Renderer/Vertex.hpp"

namespace Chicane
{
    namespace GraphicsPipeline
    {
        struct Attachment
        {
            vk::Format           format;
            vk::AttachmentLoadOp loadOp;
            vk::ImageLayout      initialLayout;
        };

        struct CreateInfo
        {
            // Modifiers
            bool                                             bHasVertices;
            bool                                             bHasDepth;
            bool                                             bHasBlending;

            // Vertex
            std::string                                      vertexShaderPath;
            std::string                                      fragmentShaderPath;
            vk::VertexInputBindingDescription                bindingDescription;
            std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;

            // Viewport
            vk::Extent2D                                     extent;

            // Atacchment
            Attachment                                       colorAttachment;
            Attachment                                       depthAttachment; // Optional if `hasDepth` == `false`

            // Pipeline Layout
            std::vector<vk::DescriptorSetLayout>             descriptorSetLayouts; // Optional if `hasDepth` == `false`
            std::vector<vk::PushConstantRange>               pushConstantRanges;

            // Vulkan
            vk::Device                                       logicalDevice;
            vk::PolygonMode                                  polygonMode;
        };

        vk::Viewport createViewport(
            const Vec<2, std::uint32_t>& inSize = Vec<2, std::uint32_t>(0),
            const Vec<2, float>& inPosition = Vec2Zero
        );
        vk::Rect2D createScissor(const Vec<2, std::uint32_t>& inSize = Vec<2, std::uint32_t>(0));
        vk::PipelineShaderStageCreateInfo createVertexShader(
            const vk::ShaderModule& inShaderModule
        );
        vk::PipelineShaderStageCreateInfo createFragmentShader(
            const vk::ShaderModule& inShaderModule
        );
        vk::PipelineVertexInputStateCreateInfo createVertexInputState(
            bool bInHasVertices,
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
            const std::vector<vk::PushConstantRange>& inPushConstantRanges,
            const vk::Device& inLogicalDevice
        );
        vk::AttachmentDescription createColorAttachment(const Attachment& inAttachment);
        vk::AttachmentReference createColorAttachmentRef();
        vk::SubpassDependency createColorSubpassDepedency();
        vk::AttachmentDescription createDepthAttachment(const Attachment& inAttachment);
        vk::AttachmentReference createDepthAttachmentRef();
        vk::SubpassDependency createDepthSubpassDepedency();
        vk::RenderPass createRendepass(
            bool bInHasDepth,
            const std::vector<vk::AttachmentDescription>& inAttachments,
            const vk::Device& inLogicalDevice
        );

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
            // Modifiers
            bool                                             m_bHasVertices;
            bool                                             m_bHasDepth;

            // Vertex
            std::string                                      m_vertexShaderPath;
            std::string                                      m_fragmentShaderPath;
            vk::VertexInputBindingDescription                m_bindingDescription;
            std::vector<vk::VertexInputAttributeDescription> m_attributeDescriptions;

            // Viewport
            vk::Extent2D                                     m_extent;

            // Layout
            std::vector<vk::DescriptorSetLayout>             m_descriptorSetLayouts;
            std::vector<vk::PushConstantRange>               m_pushConstantRanges;

            // Vulkan
            vk::Device                                       m_logicalDevice;
        };
    }
}