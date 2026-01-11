#pragma once

#include "Chicane/Runtime.hpp"

#include <vulkan/vulkan.hpp>

namespace Chicane
{
    namespace Vulkan
    {
        namespace Sync
        {
            CHICANE_RUNTIME void initSempahore(vk::Semaphore& outSemaphore, const vk::Device& inLogicalDevice);
            CHICANE_RUNTIME void initFence(vk::Fence& outFence, const vk::Device& inLogicalDevice);
        }
    }
}