#pragma once

#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Base.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/GraphicsPipeline/Attachment.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/GraphicsPipeline/CreateInfo.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/GraphicsPipeline/Instance.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace GraphicsPipeline
        {
            CHICANE vk::Viewport createViewport(
                const Vec<2, std::uint32_t>& inSize = Vec<2, std::uint32_t>(0),
                const Vec<2, float>& inPosition = Vec2Zero
            );
            CHICANE vk::Rect2D createScissor(const Vec<2, std::uint32_t>& inSize = Vec<2, std::uint32_t>(0));
            CHICANE vk::PipelineVertexInputStateCreateInfo createVertexInputState();
            CHICANE vk::PipelineInputAssemblyStateCreateInfo createInputAssemblyState();
            CHICANE vk::PipelineViewportStateCreateInfo createViewportState(
                const vk::Viewport& inViewport,
                const vk::Rect2D& inScissor
            );
            CHICANE vk::PipelineDynamicStateCreateInfo createDynamicState(const std::vector<vk::DynamicState>& inDynamicStates);
            CHICANE vk::PipelineRasterizationStateCreateInfo createRasterizationState(vk::PolygonMode inPolygonMode);
            CHICANE vk::PipelineMultisampleStateCreateInfo createMulitsampleState();
            CHICANE vk::PipelineColorBlendAttachmentState createBlendAttachmentState();
            CHICANE vk::PipelineColorBlendStateCreateInfo createColorBlendState();
            CHICANE vk::PipelineDepthStencilStateCreateInfo createDepthStencilState();
            CHICANE vk::PipelineLayout createLayout(const std::vector<vk::DescriptorSetLayout>& inDescriptorSetLayouts, const vk::Device& inLogicalDevice);
            CHICANE vk::AttachmentDescription createColorAttachment(const Attachment& inAttachment);
            CHICANE vk::SubpassDependency createColorSubpassDepedency();
            CHICANE vk::AttachmentDescription createDepthAttachment(const Attachment& inAttachment);
            CHICANE vk::SubpassDependency createDepthSubpassDepedency();
            CHICANE vk::RenderPass createRendepass(
                const std::vector<vk::AttachmentDescription>& inAttachments,
                const vk::Device& inLogicalDevice,
                bool bInHasColor,
                bool bInHasDepth
            );
        }
    }
}