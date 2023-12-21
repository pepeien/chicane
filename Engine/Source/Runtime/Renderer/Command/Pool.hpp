#pragma once

#include "Base.hpp"

#include "Renderer/Queue.hpp"

namespace Chicane
{
    namespace Command
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