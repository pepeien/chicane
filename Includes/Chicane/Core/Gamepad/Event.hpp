#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Gamepad/Axis.hpp"
#include "Chicane/Core/Gamepad/Button.hpp"
#include "Chicane/Core/Input/Status.hpp"

namespace Chicane
{
    namespace Gamepad
    {
        struct CHICANE_CORE ButtonEvent
        {
        public:
            Button        button = Button::Start;
            Input::Status status = Input::Status::Pressed;
        };
        using ButtonEventCallback = std::function<void()>;

        struct CHICANE_CORE MotionEvent
        {
        public:
            Axis          axis  = Axis::Invalid;
            std::uint16_t value = 0; /**< The axis value (range: -32768 to 32767) */
        };
        using MotionEventCallback = std::function<void(const MotionEvent&)>;
    }
}