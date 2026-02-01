#include "Chicane/Core/Input/Gamepad/Motion/Event.hpp"

#include <SDL3/SDL.h>

namespace Chicane
{
    namespace Input
    {
        GamepadMotionEvent::GamepadMotionEvent(void* inData)
            : GamepadMotionEvent()
        {
            SDL_GamepadAxisEvent event = *static_cast<SDL_GamepadAxisEvent*>(inData);

            device = event.which;
            axis   = (Input::GamepadAxis)event.axis;
            value  = static_cast<float>(event.value) / 32767.0f;
        }

        GamepadMotionEvent::GamepadMotionEvent()
            : axis(GamepadAxis::Invalid),
              value(0.0f)
        {}
    }
}