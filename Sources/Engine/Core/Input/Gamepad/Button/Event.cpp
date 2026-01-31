#include "Chicane/Core/Input/Gamepad/Button/Event.hpp"

#include <SDL3/SDL.h>

namespace Chicane
{
    namespace Input
    {
        GamepadButtonEvent::GamepadButtonEvent(void* inData)
            : GamepadButtonEvent()
        {
            SDL_GamepadButtonEvent event = *static_cast<SDL_GamepadButtonEvent*>(inData);

            device = event.which;
            button = (Input::GamepadButton)event.button;
            status = event.down ? Input::Status::Pressed : Input::Status::Released;
        }

        GamepadButtonEvent::GamepadButtonEvent()
            : button(GamepadButton::Start),
              status(Status::Pressed)
        {}
    }
}