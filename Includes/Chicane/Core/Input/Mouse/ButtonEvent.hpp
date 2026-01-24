#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Input/Device/Event.hpp"
#include "Chicane/Core/Input/Mouse/Button.hpp"
#include "Chicane/Core/Input/Status.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"

namespace Chicane
{
    namespace Input
    {
        struct CHICANE_CORE MouseButtonEvent : public DeviceEvent
        {
        public:
            MouseButton  button   = MouseButton::Left;
            Status       status   = Status::Pressed;
            std::uint8_t clicks   = 0;
            Vec2         location = Vec2::Zero;
        };
        using MouseButtonEventCallback = std::function<void()>;
    }
}