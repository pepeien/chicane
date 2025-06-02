#pragma once

#include "Chicane/Runtime/Vulkan/Essential.hpp"
#include "Chicane/Runtime/Vulkan/Queue/FamilyIndices.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Queue
        {
            CHICANE_RUNTIME void findFamilyInidices(
                FamilyIndices& outFamilyIndices,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::SurfaceKHR& inSurface
            );
        
            CHICANE_RUNTIME void initGraphicsQueue(
                vk::Queue& outQueue,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::Device& inLogicalDevice,
                const vk::SurfaceKHR& inSurface
            );
        
            CHICANE_RUNTIME void initPresentQueue(
                vk::Queue& outQueue,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::Device& inLogicalDevice,
                const vk::SurfaceKHR& inSurface
            );
        }
    }
}