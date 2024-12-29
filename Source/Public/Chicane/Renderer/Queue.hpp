#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Renderer/Queue/FamilyIndices.hpp"

namespace Chicane
{
    namespace Queue
    {
        void findFamilyInidices(
            FamilyIndices& outFamilyIndices,
            const vk::PhysicalDevice& inPhysicalDevice,
            const vk::SurfaceKHR& inSurface
        );
    
        void initGraphicsQueue(
            vk::Queue& outQueue,
            const vk::PhysicalDevice& inPhysicalDevice,
            const vk::Device& inLogicalDevice,
            const vk::SurfaceKHR& inSurface
        );
    
        void initPresentQueue(
            vk::Queue& outQueue,
            const vk::PhysicalDevice& inPhysicalDevice,
            const vk::Device& inLogicalDevice,
            const vk::SurfaceKHR& inSurface
        );
    }
}