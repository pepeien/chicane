#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
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