#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Input
    {
        // Extracted from `SDL3/SDL_GamepadButton`
        enum class GamepadButton : std::uint8_t
        {
            South,
            East,
            West,
            North,
            Back,
            Guide,
            Start,
            LeftStick,
            RightStick,
            LeftShoulder,
            RightShoulder,
            DPadUp,
            DPadDown,
            DPadLeft,
            DPadRight,
            RightPaddlePrimary,
            LeftPaddlePrimary,
            RightPaddleSecondary,
            LeftPaddleSecondary,
            Touchpad,
            Misc1,
            Misc2,
            Misc3,
            Misc4,
            Misc5,
            Misc6
        };

        inline String toString(GamepadButton inValue)
        {
            switch (inValue)
            {
            case GamepadButton::South:
                return "South";

            case GamepadButton::East:
                return "East";

            case GamepadButton::West:
                return "West";

            case GamepadButton::North:
                return "North";

            case GamepadButton::Back:
                return "Back";

            case GamepadButton::Guide:
                return "Guide";

            case GamepadButton::Start:
                return "Start";

            case GamepadButton::LeftStick:
                return "LeftStick";

            case GamepadButton::RightStick:
                return "RightStick";

            case GamepadButton::LeftShoulder:
                return "LeftShoulder";

            case GamepadButton::RightShoulder:
                return "RightShoulder";

            case GamepadButton::DPadUp:
                return "DPadUp";

            case GamepadButton::DPadDown:
                return "DPadDown";

            case GamepadButton::DPadLeft:
                return "DPadLeft";

            case GamepadButton::DPadRight:
                return "DPadRight";

            case GamepadButton::RightPaddlePrimary:
                return "RightPaddlePrimary";

            case GamepadButton::LeftPaddlePrimary:
                return "LeftPaddlePrimary";

            case GamepadButton::RightPaddleSecondary:
                return "RightPaddleSecondary";

            case GamepadButton::LeftPaddleSecondary:
                return "LeftPaddleSecondary";

            case GamepadButton::Touchpad:
                return "Touchpad";

            case GamepadButton::Misc1:
                return "Misc1";

            case GamepadButton::Misc2:
                return "Misc2";

            case GamepadButton::Misc3:
                return "Misc3";

            case GamepadButton::Misc4:
                return "Misc4";

            case GamepadButton::Misc5:
                return "Misc5";

            case GamepadButton::Misc6:
                return "Misc6";

            default:
                return "";
            }
        }
    }
}