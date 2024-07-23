#pragma once

#include "Chicane/Base.hpp"

#include "Chicane/Renderer/Queue.hpp"

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