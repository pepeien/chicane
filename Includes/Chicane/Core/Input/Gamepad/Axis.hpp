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

        inline String toString(GamepadAxis inValue)
        {
            switch (inValue)
            {
            case GamepadAxis::Invalid:
                return "Invalid";

            case GamepadAxis::LeftX:
                return "LeftX";

            case GamepadAxis::LeftY:
                return "LeftY";

            case GamepadAxis::RightX:
                return "RightX";

            case GamepadAxis::RightY:
                return "RightY";

            case GamepadAxis::LeftTrigger:
                return "LeftTrigger";

            case GamepadAxis::RightTrigger:
                return "RightTrigger";

            default:
                return "";
            }
        }
    }
}