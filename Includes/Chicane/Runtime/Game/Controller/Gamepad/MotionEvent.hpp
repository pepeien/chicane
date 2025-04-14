#pragma once

#include "Chicane.hpp"

namespace Chicane
{
    namespace Controller
    {
        namespace Gamepad
        {
            typedef SDL_GamepadAxisEvent MotionEvent;
            typedef std::function<void(const MotionEvent&)> MotionEventFunction;
        }
    }
}