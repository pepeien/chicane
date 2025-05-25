#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    namespace Gamepad
    {
        using ButtonEventData = SDL_GamepadButtonEvent;
        using ButtonEvent     = std::function<void()>;

        using MotionEventData = SDL_GamepadAxisEvent;
        using MotionEvent     = std::function<void(const MotionEventData&)>;
    }
}