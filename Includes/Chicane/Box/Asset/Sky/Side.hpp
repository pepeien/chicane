#pragma once

#include "Chicane/Box.hpp"

namespace Chicane
{
    namespace Box
    {
        enum class SkySide : std::uint8_t
        {
            Left,  // X+
            Right, // X-
            Front, // Y+
            Back,  // Y-
            Up,    // Z+
            Down   // Z-
        };
    }
}