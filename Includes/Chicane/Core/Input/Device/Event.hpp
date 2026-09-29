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
            static DeviceEvent sFromMouse(void* inData);    // From `SDL_MouseDeviceEvent`
            static DeviceEvent sFromKeyboard(void* inData); // From `SDL_KeyboardDeviceEvent`
            static DeviceEvent sFromGamepad(void* inData);  // From `SDL_GamepadDeviceEvent`

        public:
            DeviceEvent();

        public:
            DeviceID device;
        };
    }
}