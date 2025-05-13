#pragma once

#include "Chicane/Core/Essentials.hpp"

namespace Chicane
{
    namespace Input
    {
        namespace Gamepad
        {
            typedef SDL_GamepadAxisEvent MotionEvent;
            typedef std::function<void(const MotionEvent&)> MotionEventFunction;
        }
    }
}