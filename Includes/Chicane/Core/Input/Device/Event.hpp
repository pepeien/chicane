#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Input/Device.hpp"

namespace Chicane
{
    namespace Input
    {
        struct CHICANE_CORE DeviceEvent
        {
        public:
            static DeviceEvent fromMouse(void* inData);    // From `SDL_MouseDeviceEvent`
            static DeviceEvent fromKeyboard(void* inData); // From `SDL_KeyboardDeviceEvent`
            static DeviceEvent fromGamepad(void* inData);  // From `SDL_GamepadDeviceEvent`

        public:
            DeviceEvent();

        public:
            DeviceID device;
        };
    }
}