#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Gamepad/Axis.hpp"
#include "Chicane/Core/Gamepad/Button.hpp"
#include "Chicane/Core/Input.hpp"

namespace Chicane
{
    namespace Gamepad
    {
        struct CHICANE_CORE ButtonEvent : public Input::DeviceEvent
        {
        public:
            Button        button = Button::Start;
            Input::Status status = Input::Status::Pressed;
        };
        using ButtonEventCallback = std::function<void()>;

        struct CHICANE_CORE MotionEvent : public Input::DeviceEvent
        {
        public:
            Axis  axis  = Axis::Invalid;
            float value = 0.0f;
        };
        using MotionEventCallback = std::function<void(const MotionEvent&)>;
    }
}