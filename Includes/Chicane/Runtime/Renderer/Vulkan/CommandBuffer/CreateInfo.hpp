#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Runtime.hpp"

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