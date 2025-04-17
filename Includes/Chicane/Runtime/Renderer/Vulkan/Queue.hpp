#pragma once

#include "Runtime/Renderer/Vulkan/Base.hpp"
#include "Runtime/Renderer/Vulkan/Queue/FamilyIndices.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Queue
        {
            CHICANE void findFamilyInidices(
                FamilyIndices& outFamilyIndices,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::SurfaceKHR& inSurface
            );
        
            CHICANE void initGraphicsQueue(
                vk::Queue& outQueue,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::Device& inLogicalDevice,
                const vk::SurfaceKHR& inSurface
            );
        
            CHICANE void initPresentQueue(
                vk::Queue& outQueue,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::Device& inLogicalDevice,
                const vk::SurfaceKHR& inSurface
            );
        }
    }
}