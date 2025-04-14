#pragma once

#include "Chicane/Runtime/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Buffer
        {
            struct CreateInfo
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