#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

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

    inline String toString(Input::GamepadAxis inValue)
    {
        switch (inValue)
        {
        case Input::GamepadAxis::Invalid:
            return "Invalid";

        case Input::GamepadAxis::LeftX:
            return "LeftX";

        case Input::GamepadAxis::LeftY:
            return "LeftY";

        case Input::GamepadAxis::RightX:
            return "RightX";

        case Input::GamepadAxis::RightY:
            return "RightY";

        case Input::GamepadAxis::LeftTrigger:
            return "LeftTrigger";

        case Input::GamepadAxis::RightTrigger:
            return "RightTrigger";

        default:
            return "";
        }
    }
}
