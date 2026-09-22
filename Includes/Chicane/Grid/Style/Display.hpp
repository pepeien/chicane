#pragma once

#include "Chicane/Grid.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class StyleDisplay : std::uint8_t
        {
            Flex,   // Same as `Block` plus enables flex capabilities
            Block,  // Visible and occupies space on the overall layout
            Hidden, // Visibly hidden but still occupies space on the overall layout
            None    // Visibly hidden and doesn't occupiy space on the overall layout
        };
    }

    CHICANE_GRID String toString(Grid::StyleDisplay inValue);
}
