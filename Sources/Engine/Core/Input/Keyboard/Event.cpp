#include "Chicane/Core/Input/Keyboard/Event.hpp"

#include <SDL3/SDL.h>

namespace Chicane
{
    namespace Input
    {

        KeyboardEvent::KeyboardEvent(void* inData)
            : KeyboardEvent()
        {
            SDL_KeyboardEvent event = *static_cast<SDL_KeyboardEvent*>(inData);

            bIsRepeating = event.repeat;
            device       = event.which;
            button       = (Input::KeyboardButton)event.scancode;
            modifier     = (Input::KeyboardButtonModifier)event.mod;
            status       = event.down ? Input::Status::Pressed : Input::Status::Released;
            key          = event.key;
        }

        KeyboardEvent::KeyboardEvent()
            : button(KeyboardButton::Escape),
              modifier(KeyboardButtonModifier::None),
              status(Status::Pressed),
              key(0),
              bIsRepeating(false)
        {}
    }
}