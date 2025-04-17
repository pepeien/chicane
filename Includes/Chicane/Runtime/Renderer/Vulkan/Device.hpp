#pragma once

#include "Runtime/Renderer/Vulkan/Base.hpp"
#include "Runtime/Renderer/Vulkan/Queue.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Device
        {
            CHICANE bool isPhysicalDeviceSuitable(const vk::PhysicalDevice& inPhysicalDevice);

            CHICANE void pickPhysicalDevice(
                vk::PhysicalDevice& outPhysicalDevice,
                const vk::Instance& inInstance
            );

            CHICANE void initLogicalDevice(
                vk::Device& outLogicalDevice,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::SurfaceKHR& inSurface
            );
            
            CHICANE std::uint32_t findMemoryTypeIndex(
                const vk::PhysicalDevice& inPhysicalDevice,
                std::uint32_t inSupportedMemoryIndices,
                vk::MemoryPropertyFlags inRequestMemoryProperties
            );
        }
    }
}