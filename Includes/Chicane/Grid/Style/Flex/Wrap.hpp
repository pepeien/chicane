#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"
#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class StyleFlexWrap : std::uint8_t
        {
            NoWrap,
            Wrap
        };
    }

    CHICANE_GRID String toString(Grid::StyleFlexWrap inValue);
}
