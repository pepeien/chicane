#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/GraphicsPipeline/Attachment.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Shader/CreateInfo.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanGraphicsPipelineCreateInfo
        {
        public:
            // Modifiers
            bool                                          bHasVertices;
            bool                                          bHasDepthWrite;
            bool                                          bHasBlending;

            // Shader
            std::vector<VulkanShaderStageCreateInfo>      shaders;

            // Viewport
            vk::Extent2D                                  extent;

            // Attachment
            std::vector<VulkanGraphicsPipelineAttachment> attachments;

            // Rasterize
            vk::PipelineRasterizationStateCreateInfo      rasterizaterizationState;

            // Pipeline Layout
            std::vector<vk::DescriptorSetLayout>          descriptorSetLayouts;
            std::vector<vk::PushConstantRange>            pushConstantRanges;

            // Vulkan
            vk::Device                                    logicalDevice;
        };
    }
}