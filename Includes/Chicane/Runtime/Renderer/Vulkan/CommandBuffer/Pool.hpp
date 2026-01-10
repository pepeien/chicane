#pragma once

#include "Chicane/Runtime.hpp"

#include <vulkan/vulkan.hpp>

namespace Chicane
{
    namespace Vulkan
    {
        namespace CommandBuffer
        {
            namespace Pool
            {
                CHICANE_RUNTIME void init(
                    vk::CommandPool&          outCommandPool,
                    const vk::Device&         inLogicalDevice,
                    const vk::PhysicalDevice& inPhysicalDevice,
                    const vk::SurfaceKHR&     inSurface
                );
            }
        }
    }
}