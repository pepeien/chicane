#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    namespace Input
    {
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