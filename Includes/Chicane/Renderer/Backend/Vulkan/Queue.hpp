#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Queue/FamilyIndices.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace VulkanQueue
        {
            CHICANE_RENDERER void initGraphicsQueue(
                vk::Queue&                outQueue,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::Device&         inLogicalDevice,
                const vk::SurfaceKHR&     inSurface
            );

            CHICANE_RENDERER void initPresentQueue(
                vk::Queue&                outQueue,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::Device&         inLogicalDevice,
                const vk::SurfaceKHR&     inSurface
            );
        }
    }
}