#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Input/Device.hpp"
#include "Chicane/Core/Input/Keyboard/Button.hpp"
#include "Chicane/Core/Input/Keyboard/ButtonModifier.hpp"
#include "Chicane/Core/Input/Status.hpp"

namespace Chicane
{
    namespace Input
    {
        struct CHICANE_CORE KeyboardEvent : public DeviceEvent
        {
        public:
            KeyboardButton         button   = KeyboardButton::Escape;
            KeyboardButtonModifier modifier = KeyboardButtonModifier::None;
            Status                 status   = Status::Pressed;
        };
        using KeyboardEventCallback = std::function<void ()>;
    }
}