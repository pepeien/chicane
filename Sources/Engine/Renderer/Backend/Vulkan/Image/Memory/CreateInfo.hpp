#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class VulkanAllocator;

        struct CHICANE_RENDERER VulkanImageMemoryCreateInfo
        {
        public:
            vk::MemoryPropertyFlags properties;

            // Devices
            vk::Device              logicalDevice;
            vk::PhysicalDevice      physicalDevice;
            VulkanAllocator*        allocator = nullptr;
        };
    }
}