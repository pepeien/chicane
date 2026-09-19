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

    inline String toString(Grid::StyleDisplay inValue)
    {
        switch (inValue)
        {
        case Grid::StyleDisplay::Flex:
            return "Flex";

        case Grid::StyleDisplay::Block:
            return "Block";

        case Grid::StyleDisplay::Hidden:
            return "Hidden";

        case Grid::StyleDisplay::None:
            return "None";

        default:
            return "";
        }
    }
}
