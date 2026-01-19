#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace Sync
        {
            CHICANE_RENDERER void initSempahore(
                vk::Semaphore& outSemaphore, const vk::Device& inLogicalDevice
            );
            CHICANE_RENDERER void initFence(vk::Fence& outFence, const vk::Device& inLogicalDevice);
        }
    }
}