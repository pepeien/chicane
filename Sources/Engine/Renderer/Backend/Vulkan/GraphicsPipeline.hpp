#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"

#include "Chicane/Renderer.hpp"
#include "Backend/Vulkan/GraphicsPipeline/Attachment.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER VulkanGraphicsPipeline
        {
        public:
            static vk::Viewport sCreateViewport(
                const Vec<2, std::uint32_t>& inSize = Vec<2, std::uint32_t>(0), const Vec2& inPosition = Vec2::sZero()
            );
            static vk::Rect2D sCreateScissor(const Vec<2, std::uint32_t>& inSize = Vec<2, std::uint32_t>(0));
            static vk::PipelineVertexInputStateCreateInfo sCreateVertexInputState();
            static vk::PipelineInputAssemblyStateCreateInfo sCreateInputAssemblyState();
            static vk::PipelineViewportStateCreateInfo sCreateViewportState(
                const vk::Viewport& inViewport, const vk::Rect2D& inScissor
            );
            static vk::PipelineDynamicStateCreateInfo sCreateDynamicState(
                const std::vector<vk::DynamicState>& inDynamicStates
            );
            static vk::PipelineRasterizationStateCreateInfo sCreateRasterizationState(
                vk::PolygonMode inPolygonMode = vk::PolygonMode::eFill
            );
            static vk::PipelineMultisampleStateCreateInfo sCreateMulitsampleState();
            static vk::PipelineColorBlendAttachmentState sCreateBlendAttachmentState(bool bInIsEnabled = true);
            static vk::PipelineColorBlendAttachmentState sCreateAdditiveBlendAttachmentState();
            static vk::PipelineColorBlendStateCreateInfo sCreateColorBlendState();
            static vk::PipelineDepthStencilStateCreateInfo sCreateDepthStencilState();
            static vk::PipelineLayout sCreateLayout(
                const std::vector<vk::DescriptorSetLayout>& inDescriptorSetLayouts,
                const std::vector<vk::PushConstantRange>&   inPushConstants,
                const vk::Device&                           inLogicalDevice
            );
            static vk::AttachmentDescription sCreateColorAttachment(
                const VulkanGraphicsPipelineAttachment& inAttachment
            );
            static vk::SubpassDependency sCreateColorSubpassDepedency();
            static vk::AttachmentDescription sCreateDepthAttachment(
                const VulkanGraphicsPipelineAttachment& inAttachment
            );
            static vk::SubpassDependency sCreateDepthSubpassDepedency();
            static vk::RenderPass sCreateRendepass(
                const std::vector<vk::AttachmentDescription>& inAttachments,
                const std::vector<vk::SubpassDependency>&     inSubpassDepedencies,
                const std::vector<vk::SubpassDescription>&    inSubpasses,
                const vk::Device&                             inLogicalDevice
            );

        public:
            void init(const vk::Device& inLogicalDevice, const vk::GraphicsPipelineCreateInfo& inCreateInfo);
            void bind(vk::CommandBuffer& inCommandBuffer);
            void bind(vk::CommandBuffer& inCommandBuffer, std::uint32_t inIndex, vk::DescriptorSet inDescriptorSet);
            void destroy();

        public:
            vk::PipelineLayout layout;
            vk::RenderPass     renderPass;
            vk::Pipeline       instance;

        private:
            vk::Device m_logicalDevice;
        };
    }
}