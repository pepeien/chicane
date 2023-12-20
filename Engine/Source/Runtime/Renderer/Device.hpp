#pragma once

#include "Base.hpp"

#include "Queue.hpp"

namespace Engine
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
        
        uint32_t findMemoryTypeIndex(
            const vk::PhysicalDevice& inPhysicalDevice,
            const uint32_t& inSupportedMemoryIndices,
            vk::MemoryPropertyFlags inRequestMemoryProperties
        );
    }
}