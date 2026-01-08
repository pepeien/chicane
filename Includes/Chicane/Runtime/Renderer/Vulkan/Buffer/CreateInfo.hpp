#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        struct CHICANE_RUNTIME BufferCreateInfo
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