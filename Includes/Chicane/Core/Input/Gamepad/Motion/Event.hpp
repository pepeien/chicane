#pragma once

#include <functional>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Input/Device/Event.hpp"
#include "Chicane/Core/Input/Gamepad/Axis.hpp"

namespace Chicane
{
    namespace Input
    {
        struct CHICANE_CORE GamepadMotionEvent : public DeviceEvent
        {
        public:
            GamepadMotionEvent(void* inData); // From `SDL_GamepadAxisEvent`
            GamepadMotionEvent();

        public:
            GamepadAxis axis  = GamepadAxis::Invalid;
            float       value = 0.0f;
        };

        using GamepadMotionEventCallback = std::function<void(const GamepadMotionEvent&)>;
    }
}