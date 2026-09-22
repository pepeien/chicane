#include "Chicane/Core/Input/Gamepad/Button.hpp"

namespace Chicane
{
    String toString(Input::GamepadButton inValue)
    {
        switch (inValue)
        {
        case Input::GamepadButton::South:
            return "South";

        case Input::GamepadButton::East:
            return "East";

        case Input::GamepadButton::West:
            return "West";

        case Input::GamepadButton::North:
            return "North";

        case Input::GamepadButton::Back:
            return "Back";

        case Input::GamepadButton::Guide:
            return "Guide";

        case Input::GamepadButton::Start:
            return "Start";

        case Input::GamepadButton::LeftStick:
            return "LeftStick";

        case Input::GamepadButton::RightStick:
            return "RightStick";

        case Input::GamepadButton::LeftShoulder:
            return "LeftShoulder";

        case Input::GamepadButton::RightShoulder:
            return "RightShoulder";

        case Input::GamepadButton::DPadUp:
            return "DPadUp";

        case Input::GamepadButton::DPadDown:
            return "DPadDown";

        case Input::GamepadButton::DPadLeft:
            return "DPadLeft";

        case Input::GamepadButton::DPadRight:
            return "DPadRight";

        case Input::GamepadButton::RightPaddlePrimary:
            return "RightPaddlePrimary";

        case Input::GamepadButton::LeftPaddlePrimary:
            return "LeftPaddlePrimary";

        case Input::GamepadButton::RightPaddleSecondary:
            return "RightPaddleSecondary";

        case Input::GamepadButton::LeftPaddleSecondary:
            return "LeftPaddleSecondary";

        case Input::GamepadButton::Touchpad:
            return "Touchpad";

        case Input::GamepadButton::Misc1:
            return "Misc1";

        case Input::GamepadButton::Misc2:
            return "Misc2";

        case Input::GamepadButton::Misc3:
            return "Misc3";

        case Input::GamepadButton::Misc4:
            return "Misc4";

        case Input::GamepadButton::Misc5:
            return "Misc5";

        case Input::GamepadButton::Misc6:
            return "Misc6";

        default:
            return "";
        }
    }
}
