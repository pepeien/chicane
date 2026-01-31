#pragma once

#include <functional>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Input/Device/Event.hpp"
#include "Chicane/Core/Input/Keyboard/Button.hpp"
#include "Chicane/Core/Input/Keyboard/Button/Modifier.hpp"
#include "Chicane/Core/Input/Status.hpp"

namespace Chicane
{
    namespace Input
    {
        struct CHICANE_CORE KeyboardEvent : public DeviceEvent
        {
        public:
            KeyboardEvent(void* inData); // From `SDL_KeyboardEvent`
            KeyboardEvent();

        public:
            KeyboardButton         button;
            KeyboardButtonModifier modifier;
            Status                 status;
        };

        using KeyboardEventCallback = std::function<void()>;
    }
}