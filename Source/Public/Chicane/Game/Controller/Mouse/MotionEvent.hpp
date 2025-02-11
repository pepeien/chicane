#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Controller/Event.hpp"

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