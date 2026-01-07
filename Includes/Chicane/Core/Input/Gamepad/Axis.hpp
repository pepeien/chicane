#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    namespace Input
    {
        // Extracted from `SDL3/SDL_GamepadAxis`
        enum class GamepadAxis : int
        {
            Invalid = -1,
            LeftX,
            LeftY,
            RightX,
            RightY,
            LeftTrigger,
            RightTrigger
        };
    }
}