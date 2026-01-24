#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"

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
    }
}