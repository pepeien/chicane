#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Sync
    {
        void initSempahore(vk::Semaphore& outSemaphore, const vk::Device& inLogicalDevice);
        void initFence(vk::Fence& outFence, const vk::Device& inLogicalDevice);
    }
}