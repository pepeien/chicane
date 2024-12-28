#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Image
    {
        struct CreateInfo
        {
        public:
            // Settings
            uint32_t                width;
			uint32_t                height;
            uint32_t                count;

            // Vulkan
			vk::Device              logicalDevice;
			vk::PhysicalDevice      physicalDevice;
            vk::ImageTiling         tiling;
            vk::ImageUsageFlags     usage;
            vk::ImageCreateFlags    create;
            vk::MemoryPropertyFlags memoryProperties;
            vk::Format              format;
        }; 
    }
}