#pragma once

#include <functional>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Input/Device/Event.hpp"
#include "Chicane/Core/Input/Gamepad/Button.hpp"
#include "Chicane/Core/Input/Status.hpp"

namespace Chicane
{
    namespace Input
    {
        struct CHICANE_CORE GamepadButtonEvent : public DeviceEvent
        {
        public:
            GamepadButtonEvent(void* inData); // From `SDL_GamepadButtonEvent`
            GamepadButtonEvent();

        public:
            GamepadButton button;
            Status        status;
        };

        using GamepadButtonEventCallback = std::function<void()>;
    }
}