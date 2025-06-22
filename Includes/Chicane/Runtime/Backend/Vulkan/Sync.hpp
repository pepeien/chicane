#pragma once

#include "Chicane/Runtime/Backend/Vulkan/Essential.hpp"

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