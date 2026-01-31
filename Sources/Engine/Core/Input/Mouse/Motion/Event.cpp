#include "Chicane/Core/Input/Mouse/Motion/Event.hpp"

#include <SDL3/SDL.h>

namespace Chicane
{
    namespace Input
    {
        MouseMotionEvent::MouseMotionEvent(void* inData)
            : MouseMotionEvent()
        {
            SDL_MouseMotionEvent event = *static_cast<SDL_MouseMotionEvent*>(inData);

            device             = event.which;
            state              = event.state;
            location.x         = event.x;
            location.y         = event.y;
            relativeLocation.x = event.xrel;
            relativeLocation.y = event.yrel;
        }

        MouseMotionEvent::MouseMotionEvent()
            : state(0U),
              location(Vec2::Zero),
              relativeLocation(Vec2::Zero)
        {}
    }
}