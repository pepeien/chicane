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
            MouseButtonEvent(void* inData); // From `SDL_MouseButtonEvent`
            MouseButtonEvent();

        public:
            MouseButton  button;
            Status       status;
            std::uint8_t clicks;
            Vec2         location;
        };

        using MouseButtonEventCallback = std::function<void()>;
    }
}