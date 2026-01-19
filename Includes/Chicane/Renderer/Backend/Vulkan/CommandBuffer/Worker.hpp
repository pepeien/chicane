#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Core/String.hpp"

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace VulkanCommandBufferWorker
        {
            CHICANE_RENDERER void startJob(const vk::CommandBuffer& inCommandBuffer);
            CHICANE_RENDERER void endJob(
                const vk::CommandBuffer& inCommandBuffer,
                const vk::Queue&         inQueue,
                const String&            inDescription = "Command Buffer"
            );
            CHICANE_RENDERER void endJob(
                const vk::Queue&      inQueue,
                const vk::SubmitInfo& inSubmitInfo,
                const String&         inDescription = "Command Buffer"
            );
        }
    }
}