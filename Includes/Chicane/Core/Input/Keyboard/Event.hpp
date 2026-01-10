#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Input/Device/Event.hpp"
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
        using KeyboardEventCallback = std::function<void()>;
    }
}