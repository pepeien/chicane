#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace VulkanCommandBufferPool
        {
            CHICANE_RENDERER void init(
                vk::CommandPool&          outCommandPool,
                const vk::Device&         inLogicalDevice,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::SurfaceKHR&     inSurface
            );
        }
    }