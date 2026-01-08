#pragma once

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
            GamepadButton button = GamepadButton::Start;
            Status        status = Status::Pressed;
        };
        using GamepadButtonEventCallback = std::function<void()>;
    }
}