#pragma once

#include "Base.hpp"

namespace Engine
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