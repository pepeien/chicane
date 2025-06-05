#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Input.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Mouse/Button.hpp"

namespace Chicane
{
    namespace Mouse
    {
        struct CHICANE_CORE ButtonEvent : public Input::DeviceEvent
        {
        public:
            Button        button   = Button::Left;
            Input::Status status   = Input::Status::Pressed;
            std::uint8_t  clicks   = 0;
            Vec2          location = Vec2::Zero;
        };
        using ButtonEventCallback = std::function<void()>;

        struct CHICANE_CORE MotionEvent : public Input::DeviceEvent
        {
        public:
            Input::Status status           = Input::Status::Pressed;
            Vec2          location         = Vec2::Zero;
            Vec2          relativeLocation = Vec2::Zero;
        };
        using MotionEventCallback = std::function<void(const MotionEvent&)>;
    }
}