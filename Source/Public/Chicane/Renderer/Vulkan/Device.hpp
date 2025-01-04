#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"
#include "Chicane/Renderer/Vulkan/Queue.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Device
        {
            bool isPhysicalDeviceSuitable(const vk::PhysicalDevice& inPhysicalDevice);

            void pickPhysicalDevice(
                vk::PhysicalDevice& outPhysicalDevice,
                const vk::Instance& inInstance
            );

            void initLogicalDevice(
                vk::Device& outLogicalDevice,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::SurfaceKHR& inSurface
            );
            
            std::uint32_t findMemoryTypeIndex(
                const vk::PhysicalDevice& inPhysicalDevice,
                std::uint32_t inSupportedMemoryIndices,
                vk::MemoryPropertyFlags inRequestMemoryProperties
            );
        }
    }
}