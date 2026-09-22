#pragma once

#include <cstdint>

#include "Chicane/Grid.hpp"
#include "Chicane/Core/String.hpp"

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

    CHICANE_GRID String toString(Grid::DockSide inValue);
}
