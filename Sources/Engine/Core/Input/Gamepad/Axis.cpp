#include "Chicane/Core/Input/Gamepad/Axis.hpp"

namespace Chicane
{
    String toString(Input::GamepadAxis inValue)
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
