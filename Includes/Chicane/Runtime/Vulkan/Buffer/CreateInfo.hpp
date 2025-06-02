#pragma once

#include "Chicane/Runtime/Vulkan/Essential.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Buffer
        {
            struct CHICANE_RUNTIME CreateInfo
            {
            public:
                vk::DeviceSize          size;
                vk::BufferUsageFlags    usage;
                vk::Device              logicalDevice;
                vk::PhysicalDevice      physicalDevice;
                vk::MemoryPropertyFlags memoryProperties;
            };
        }
    }
}