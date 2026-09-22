#pragma once

#include <cstdint>

#include "Chicane/Grid.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class SvgLineCap : std::uint8_t
        {
            Butt,
            Round,
            Square
        };
    }

    CHICANE_GRID String toString(Grid::SvgLineCap inValue);
}
