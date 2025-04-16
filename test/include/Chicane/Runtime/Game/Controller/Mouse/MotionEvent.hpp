#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Runtime/Game/Controller/Event.hpp"

namespace Chicane
{
    namespace Controller
    {
        namespace Mouse
        {
            typedef SDL_MouseMotionEvent MotionEvent;
            typedef std::function<void(const MotionEvent&)> MotionEventFunction;
        }
    }
}