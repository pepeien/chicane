#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    namespace Input
    {
        namespace Mouse
        {
            using MotionEvent         = SDL_MouseMotionEvent;
            using MotionEventFunction = std::function<void(const MotionEvent&)>;
        }
    }
}