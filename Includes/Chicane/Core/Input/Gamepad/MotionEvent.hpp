#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    namespace Input
    {
        namespace Gamepad
        {
            using MotionEvent         = SDL_GamepadAxisEvent;
            using MotionEventFunction = std::function<void(const MotionEvent&)>;
        }
    }
}