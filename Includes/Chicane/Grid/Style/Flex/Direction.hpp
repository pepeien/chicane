#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"
#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class StyleFlexDirection : std::uint8_t
        {
            Column,
            Row
        };
    }

    CHICANE_GRID String toString(Grid::StyleFlexDirection inValue);
}
