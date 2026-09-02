#pragma once

#include <cstdint>

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class DockSide : std::uint8_t
        {
            Fill,
            Float,
            Left,
            Right,
            Top,
            Bottom
        };
    }
}
