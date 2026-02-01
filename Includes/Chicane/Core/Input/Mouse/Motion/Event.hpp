#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Input/Device/Event.hpp"
#include "Chicane/Core/Input/Status.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"

namespace Chicane
{
    namespace Input
    {
        struct CHICANE_CORE MouseMotionEvent : public DeviceEvent
        {
        public:
            MouseMotionEvent(void* inData); // From `SDL_MouseMotionEvent`
            MouseMotionEvent();

        public:
            std::uint32_t state            = 0U;
            Vec2          location         = Vec2::Zero;
            Vec2          relativeLocation = Vec2::Zero;
        };

        using MouseMotionEventCallback = std::function<void(const MouseMotionEvent&)>;
    }
}