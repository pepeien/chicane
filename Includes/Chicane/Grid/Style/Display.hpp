#pragma once

#include "Chicane/Grid/Essential.hpp"

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
}