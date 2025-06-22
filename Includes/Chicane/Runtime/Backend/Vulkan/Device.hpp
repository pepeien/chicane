#pragma once

#include "Chicane/Runtime/Backend/Vulkan/Essential.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Device
        {
            static const std::vector<const char*> LAYERS = {};

            static const std::vector<const char*> EXTENSIONS = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
            };

            CHICANE_RUNTIME bool isPhysicalDeviceSuitable(const vk::PhysicalDevice& inDevice);

            CHICANE_RUNTIME void pickPhysicalDevice(
                vk::PhysicalDevice& outDevice,
                const vk::Instance& inInstance
            );

            CHICANE_RUNTIME void initLogicalDevice(
                vk::Device& outDevice,
                const vk::PhysicalDevice& inDevice,
                const vk::SurfaceKHR& inSurface
            );

            CHICANE_RUNTIME std::uint32_t findMemoryTypeIndex(
                const vk::PhysicalDevice& inDevice,
                std::uint32_t inSupportedMemoryIndices,
                vk::MemoryPropertyFlags inRequestMemoryProperties
            );
        }
    }
}