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
    }

    inline String toString(Box::SkySide inValue)
    {
        switch (inValue)
        {
        case Box::SkySide::Right:
            return "Right";

        case Box::SkySide::Left:
            return "Left";

        case Box::SkySide::Front:
            return "Front";

        case Box::SkySide::Back:
            return "Back";

        case Box::SkySide::Up:
            return "Up";

        case Box::SkySide::Down:
            return "Down";

        default:
            return "";
        }
    }
}
