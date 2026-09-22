#pragma once

#include <cstdint>

#include "Chicane/Grid.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class SvgLineJoin : std::uint8_t
        {
            Miter,
            Round,
            Bevel
        };
    }

    CHICANE_GRID String toString(Grid::SvgLineJoin inValue);
}
