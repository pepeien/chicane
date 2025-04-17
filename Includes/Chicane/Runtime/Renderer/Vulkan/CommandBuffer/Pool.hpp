#pragma once

#include "Runtime/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace CommandBuffer
        {
            namespace Pool
            {
                CHICANE void init(
                    vk::CommandPool& outCommandPool,
                    const vk::Device& inLogicalDevice,
                    const vk::PhysicalDevice& inPhysicalDevice,
                    const vk::SurfaceKHR& inSurface
                );
            }
        }
    }
}