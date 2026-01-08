#pragma once

#include "Chicane/Box.hpp"

namespace Chicane
{
    namespace Box
    {
        enum class SkySide : std::uint8_t
        {
            Left,
            Right,
            Front,
            Back,
            Up,
            Down
        };
    }
}