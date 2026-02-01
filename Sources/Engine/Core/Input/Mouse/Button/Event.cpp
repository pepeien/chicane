#include "Chicane/Core/Input/Mouse/Button/Event.hpp"

#include <SDL3/SDL.h>

namespace Chicane
{
    namespace Input
    {
        MouseButtonEvent::MouseButtonEvent(void* inData)
            : MouseButtonEvent()
        {
            SDL_MouseButtonEvent event = *static_cast<SDL_MouseButtonEvent*>(inData);

            device     = event.which;
            button     = (Input::MouseButton)event.button;
            status     = event.down ? Input::Status::Pressed : Input::Status::Released;
            clicks     = event.clicks;
            location.x = event.x;
            location.y = event.y;
        }

        MouseButtonEvent::MouseButtonEvent()
            : button(MouseButton::Left),
              status(Status::Pressed),
              clicks(0U),
              location(Vec2::Zero)
        {}
    }
}