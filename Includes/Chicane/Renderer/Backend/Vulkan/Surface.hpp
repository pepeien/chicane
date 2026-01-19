#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace VulkanSurface
        {
            CHICANE_RENDERER void init(
                vk::SurfaceKHR& outSurface, const vk::Instance& inInstance, void* inWindow
            );
        }
    }
}