#pragma once

#include "Chicane/Runtime/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Sync
        {
            void initSempahore(vk::Semaphore& outSemaphore, const vk::Device& inLogicalDevice);
            void initFence(vk::Fence& outFence, const vk::Device& inLogicalDevice);
        }
    }
}