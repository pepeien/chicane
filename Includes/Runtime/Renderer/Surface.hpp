#pragma once

#include "Runtime/Runtime.hpp"

namespace Chicane
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