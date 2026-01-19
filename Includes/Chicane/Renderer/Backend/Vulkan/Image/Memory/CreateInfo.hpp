#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanImageMemoryCreateInfo
        {
        public:
            vk::MemoryPropertyFlags properties;

            // Devices
            vk::Device              logicalDevice;
            vk::PhysicalDevice      physicalDevice;
        };
    }
}