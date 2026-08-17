#include "Chicane/Core/Input/Mouse/Wheel/Event.hpp"

#include <SDL3/SDL.h>

namespace Chicane
{
    namespace Input
    {
        MouseWheelEvent::MouseWheelEvent(void* inData)
            : MouseWheelEvent()
        {
            SDL_MouseWheelEvent event = *static_cast<SDL_MouseWheelEvent*>(inData);

            device     = event.which;
            delta.x    = event.x;
            delta.y    = event.y;
            location.x = event.mouse_x;
            location.y = event.mouse_y;

            if (event.direction == SDL_MOUSEWHEEL_FLIPPED)
            {
                delta.x *= -1.0f;
                delta.y *= -1.0f;
            }
        }

        MouseWheelEvent::MouseWheelEvent()
            : delta(Vec2::Zero()),
              location(Vec2::Zero())
        {}
    }
}
