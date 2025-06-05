#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Input.hpp"
#include "Chicane/Core/Keyboard/Button.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"

namespace Chicane
{
    namespace Keyboard
    {
        struct CHICANE_CORE Event : public Input::DeviceEvent
        {
        public:
            Button         button   = Button::Escape;
            ButtonModifier modifier = ButtonModifier::None;
            Input::Status  status   = Input::Status::Pressed;
        };
        using EventCallback = std::function<void ()>;
    }
}