#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    namespace Mouse
    {
        using ButtonEventData     = SDL_MouseButtonEvent;
        using ButtonEvent         = std::function<void()>;

        using MotionEventData     = SDL_MouseMotionEvent;
        using MotionEvent         = std::function<void(const MotionEventData&)>;
    }
}