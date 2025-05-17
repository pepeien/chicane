#pragma once

#include "Chicane/Runtime/Renderer/Vulkan/Essential.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/GraphicsPipeline/Attachment.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/GraphicsPipeline/CreateInfo.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/GraphicsPipeline/Instance.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace GraphicsPipeline
        {
            CHICANE_RUNTIME vk::Viewport createViewport(
                const Vec<2, std::uint32_t>& inSize = Vec<2, std::uint32_t>(0),
                const Vec<2, float>& inPosition = Vec2Zero
            );
            CHICANE_RUNTIME vk::Rect2D createScissor(const Vec<2, std::uint32_t>& inSize = Vec<2, std::uint32_t>(0));
            CHICANE_RUNTIME vk::PipelineVertexInputStateCreateInfo createVertexInputState();
            CHICANE_RUNTIME vk::PipelineInputAssemblyStateCreateInfo createInputAssemblyState();
            CHICANE_RUNTIME vk::PipelineViewportStateCreateInfo createViewportState(
                const vk::Viewport& inViewport,
                const vk::Rect2D& inScissor
            );
            CHICANE_RUNTIME vk::PipelineDynamicStateCreateInfo createDynamicState(const std::vector<vk::DynamicState>& inDynamicStates);
            CHICANE_RUNTIME vk::PipelineRasterizationStateCreateInfo createRasterizationState(vk::PolygonMode inPolygonMode);
            CHICANE_RUNTIME vk::PipelineMultisampleStateCreateInfo createMulitsampleState();
            CHICANE_RUNTIME vk::PipelineColorBlendAttachmentState createBlendAttachmentState();
            CHICANE_RUNTIME vk::PipelineColorBlendStateCreateInfo createColorBlendState();
            CHICANE_RUNTIME vk::PipelineDepthStencilStateCreateInfo createDepthStencilState();
            CHICANE_RUNTIME vk::PipelineLayout createLayout(
                const std::vector<vk::DescriptorSetLayout>& inDescriptorSetLayouts,
                const std::vector<vk::PushConstantRange>& inPushConstants,
                const vk::Device& inLogicalDevice
            );
            CHICANE_RUNTIME vk::AttachmentDescription createColorAttachment(const Attachment& inAttachment);
            CHICANE_RUNTIME vk::SubpassDependency createColorSubpassDepedency();
            CHICANE_RUNTIME vk::AttachmentDescription createDepthAttachment(const Attachment& inAttachment);
            CHICANE_RUNTIME vk::SubpassDependency createDepthSubpassDepedency();
            CHICANE_RUNTIME vk::RenderPass createRendepass(
                const std::vector<vk::AttachmentDescription>& inAttachments,
                const vk::Device& inLogicalDevice,
                bool bInHasColor,
                bool bInHasDepth
            );
        }
    }
}