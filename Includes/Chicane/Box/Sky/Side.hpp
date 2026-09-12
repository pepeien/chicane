#pragma once

#include "Chicane/Box.hpp"

namespace Chicane
{
    namespace Box
    {
        enum class SkySide : std::uint8_t
        {
            Right, // X+
            Left,  // X-
            Front, // Y+
            Back,  // Y-
            Up,    // Z+
            Down   // Z-
        };

        inline String toString(SkySide inValue)
        {
            switch (inValue)
            {
            case SkySide::Right:
                return "Right";

            case SkySide::Left:
                return "Left";

            case SkySide::Front:
                return "Front";

            case SkySide::Back:
                return "Back";

            case SkySide::Up:
                return "Up";

            case SkySide::Down:
                return "Down";

            default:
                return "";
            }
        }
    }
}