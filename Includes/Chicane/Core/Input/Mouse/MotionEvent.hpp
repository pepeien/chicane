#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Input/Device.hpp"
#include "Chicane/Core/Input/Status.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"

namespace Chicane
{
    namespace Input
    {
        struct CHICANE_CORE MouseMotionEvent : public DeviceEvent
        {
        public:
            Status status           = Status::Pressed;
            Vec2   location         = Vec2::Zero;
            Vec2   relativeLocation = Vec2::Zero;
        };
        using MouseMotionEventCallback = std::function<void(const MouseMotionEvent&)>;
    }
}