#pragma once

#include <cstdint>

#include "Chicane/Grid.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class StyleBorderType : std::uint8_t
        {
            None,
            Solid
        };
    }

    CHICANE_GRID String toString(Grid::StyleBorderType inValue);
}
