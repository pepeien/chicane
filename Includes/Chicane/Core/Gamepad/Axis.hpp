#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    namespace Gamepad
    {
        enum class Axis : int
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