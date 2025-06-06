#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    namespace Input
    {
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
    }
}