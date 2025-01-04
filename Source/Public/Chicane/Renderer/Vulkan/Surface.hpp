#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Surface
        {
            void init(
                vk::SurfaceKHR& outSurface,
                const vk::Instance& inInstance,
                SDL_Window* inWindow
            );
        }
    }
}