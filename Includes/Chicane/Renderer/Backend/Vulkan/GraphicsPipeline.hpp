#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/GraphicsPipeline/CreateInfo.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER VulkanGraphicsPipeline
        {
        public:
            static vk::Viewport createViewport(
                const Vec<2, std::uint32_t>& inSize = Vec<2, std::uint32_t>(0), const Vec2& inPosition = Vec2::Zero
            );
            static vk::Rect2D createScissor(const Vec<2, std::uint32_t>& inSize = Vec<2, std::uint32_t>(0));
            static vk::PipelineVertexInputStateCreateInfo createVertexInputState();
            static vk::PipelineInputAssemblyStateCreateInfo createInputAssemblyState();
            static vk::PipelineViewportStateCreateInfo createViewportState(
                const vk::Viewport& inViewport, const vk::Rect2D& inScissor
            );
            static vk::PipelineDynamicStateCreateInfo createDynamicState(
                const std::vector<vk::DynamicState>& inDynamicStates
            );
            static vk::PipelineRasterizationStateCreateInfo createRasterizationState(vk::PolygonMode inPolygonMode);
            static vk::PipelineMultisampleStateCreateInfo createMulitsampleState();
            static vk::PipelineColorBlendAttachmentState createBlendAttachmentState();
            static vk::PipelineColorBlendStateCreateInfo createColorBlendState();
            static vk::PipelineDepthStencilStateCreateInfo createDepthStencilState();
            static vk::PipelineLayout createLayout(
                const std::vector<vk::DescriptorSetLayout>& inDescriptorSetLayouts,
                const std::vector<vk::PushConstantRange>&   inPushConstants,
                const vk::Device&                           inLogicalDevice
            );
            static vk::AttachmentDescription createColorAttachment(
                const VulkanGraphicsPipelineAttachment& inAttachment
            );
            static vk::SubpassDependency createColorSubpassDepedency();
            static vk::AttachmentDescription createDepthAttachment(
                const VulkanGraphicsPipelineAttachment& inAttachment
            );
            static vk::SubpassDependency createDepthSubpassDepedency();
            static vk::RenderPass createRendepass(
                const std::vector<vk::AttachmentDescription>& inAttachments,
                const vk::Device&                             inLogicalDevice,
                bool                                          bInHasColor,
                bool                                          bInHasDepth
            );

        public:
            VulkanGraphicsPipeline(const VulkanGraphicsPipelineCreateInfo& inCreateInfo);
            ~VulkanGraphicsPipeline();

        public:
            void bind(vk::CommandBuffer& inCommandBuffer);
            void bindDescriptorSet(
                vk::CommandBuffer& inCommandBuffer, std::uint32_t inIndex, vk::DescriptorSet inDescriptorSet
            );

        public:
            vk::PipelineLayout layout;
            vk::RenderPass     renderPass;
            vk::Pipeline       instance;

        private:
            vk::Device m_logicalDevice;
        };
    }
}