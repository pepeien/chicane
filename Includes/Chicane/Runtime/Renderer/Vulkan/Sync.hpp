#pragma once

#include "Chicane/Runtime/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Sync
        {
            CHICANE void initSempahore(vk::Semaphore& outSemaphore, const vk::Device& inLogicalDevice);
            CHICANE void initFence(vk::Fence& outFence, const vk::Device& inLogicalDevice);
        }
    }
}