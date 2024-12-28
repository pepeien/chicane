#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace CommandBuffer
    {
        namespace Pool
        {
            void init(
                vk::CommandPool& outCommandPool,
                const vk::Device& inLogicalDevice,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::SurfaceKHR& inSurface
            );
        }
    }
}