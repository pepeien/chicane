#pragma once

#include "Chicane/Runtime/Vulkan/Essential.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace CommandBuffer
        {
            struct CHICANE_RUNTIME CreateInfo
            {
            public:
                vk::Device      logicalDevice;
                vk::CommandPool commandPool;
            };
        }
    }
}