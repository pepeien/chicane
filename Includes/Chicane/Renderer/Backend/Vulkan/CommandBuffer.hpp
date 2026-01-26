#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/CommandBuffer/CreateInfo.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace VulkanCommandBuffer
        {
            CHICANE_RENDERER void init(
                std::vector<vk::CommandBuffer>& outCommandBuffers, const VulkanCommandBufferCreateInfo& inCreateInfo
            );
            CHICANE_RENDERER void init(
                vk::CommandBuffer& outCommandBuffer, const VulkanCommandBufferCreateInfo& inCreateInfo
            );
        }
    }
}