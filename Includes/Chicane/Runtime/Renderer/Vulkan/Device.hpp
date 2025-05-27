#pragma once

#include "Chicane/Runtime/Renderer/Vulkan/Essential.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Queue.hpp"

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

            CHICANE_RUNTIME bool isPhysicalDeviceSuitable(const vk::PhysicalDevice& inPhysicalDevice);

            CHICANE_RUNTIME void pickPhysicalDevice(
                vk::PhysicalDevice& outPhysicalDevice,
                const vk::Instance& inInstance
            );

            CHICANE_RUNTIME void initLogicalDevice(
                vk::Device& outLogicalDevice,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::SurfaceKHR& inSurface
            );
            
            CHICANE_RUNTIME std::uint32_t findMemoryTypeIndex(
                const vk::PhysicalDevice& inPhysicalDevice,
                std::uint32_t inSupportedMemoryIndices,
                vk::MemoryPropertyFlags inRequestMemoryProperties
            );
        }
    }
}