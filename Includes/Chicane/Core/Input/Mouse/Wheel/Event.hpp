#pragma once

#include <functional>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Input/Device/Event.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"

namespace Chicane
{
    namespace Input
    {
        struct CHICANE_CORE MouseWheelEvent : public DeviceEvent
        {
        public:
            MouseWheelEvent(void* inData); // From `SDL_MouseWheelEvent`
            MouseWheelEvent();

        public:
            Vec2 delta    = Vec2::Zero();
            Vec2 location = Vec2::Zero();
        };

        using MouseWheelEventCallback = std::function<void(const MouseWheelEvent&)>;
    }
}
