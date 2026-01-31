#include "Chicane/Core/Input/Device/Event.hpp"

#include <limits>

#include <SDL3/SDL.h>

namespace Chicane
{
    namespace Input
    {
        DeviceEvent DeviceEvent::fromMouse(void* inData)
        {
            SDL_MouseDeviceEvent event = *static_cast<SDL_MouseDeviceEvent*>(inData);

            DeviceEvent result;
            result.device = event.which;

            return result;
        }

        DeviceEvent DeviceEvent::fromKeyboard(void* inData)
        {
            SDL_KeyboardDeviceEvent event = *static_cast<SDL_KeyboardDeviceEvent*>(inData);

            DeviceEvent result;
            result.device = event.which;

            return result;
        }

        DeviceEvent DeviceEvent::fromGamepad(void* inData)
        {
            SDL_GamepadDeviceEvent event = *static_cast<SDL_GamepadDeviceEvent*>(inData);

            DeviceEvent result;
            result.device = event.which;

            return result;
        }

        DeviceEvent::DeviceEvent()
            : device(std::numeric_limits<std::uint32_t>::max())
        {}
    }
}