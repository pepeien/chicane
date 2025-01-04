#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"
#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Renderer/Vulkan/GraphicsPipeline/Attachment.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace GraphicsPipeline
        {
            vk::Viewport createViewport(
                const Vec<2, std::uint32_t>& inSize = Vec<2, std::uint32_t>(0),
                const Vec<2, float>& inPosition = Vec2Zero
            );
            vk::Rect2D createScissor(const Vec<2, std::uint32_t>& inSize = Vec<2, std::uint32_t>(0));
            vk::PipelineShaderStageCreateInfo createVertexShader(const vk::ShaderModule& inShaderModule);
            vk::PipelineShaderStageCreateInfo createFragmentShader(const vk::ShaderModule& inShaderModule);
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
        }
    }
}