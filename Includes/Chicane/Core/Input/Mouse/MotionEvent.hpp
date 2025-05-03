#pragma once

#include "Core/Essentials.hpp"

namespace Chicane
{
    namespace Input
    {
        namespace Mouse
        {
            typedef SDL_MouseMotionEvent MotionEvent;
            typedef std::function<void(const MotionEvent&)> MotionEventFunction;
        }
    }
}