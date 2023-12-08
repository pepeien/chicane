#pragma once

#include "Base.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            namespace Sync
            {
                void initSempahore(vk::Semaphore& outSemaphore, vk::Device& inLogicalDevice);
                void initFence(vk::Fence& outFence, vk::Device& inLogicalDevice);
            }
        }
    }
}