#pragma once

#include <cstdint>

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace VulkanDevice
        {
            static const std::vector<const char*> LAYERS     = {};

            static const std::vector<const char*> EXTENSIONS = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_ROBUSTNESS_2_EXTENSION_NAME
            };

            CHICANE_RENDERER bool isPhysicalDeviceSuitable(const vk::PhysicalDevice& inDevice);

            CHICANE_RENDERER void pickPhysicalDevice(vk::PhysicalDevice& outDevice, const vk::Instance& inInstance);

            CHICANE_RENDERER void initLogicalDevice(
                vk::Device& outDevice, const vk::PhysicalDevice& inDevice, const vk::SurfaceKHR& inSurface
            );

            CHICANE_RENDERER std::uint32_t findMemoryTypeIndex(
                const vk::PhysicalDevice& inDevice,
                std::uint32_t             inSupportedMemoryIndices,
                vk::MemoryPropertyFlags   inRequestMemoryProperties
            );
        }
    }
}