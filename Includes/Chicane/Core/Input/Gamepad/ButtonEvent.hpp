#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Input/Device.hpp"
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