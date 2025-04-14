#pragma once

#include "Chicane/Runtime/Renderer/Vulkan/Base.hpp"

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