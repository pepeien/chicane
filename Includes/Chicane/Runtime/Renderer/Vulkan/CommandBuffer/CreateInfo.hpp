#pragma once

#include "Chicane/Runtime.hpp"

#include <vulkan/vulkan.hpp>

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