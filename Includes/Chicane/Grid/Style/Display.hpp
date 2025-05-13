#pragma once

#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            enum class Display : std::uint8_t
            {
                Flex,    // Same as `Visible` plus enables flex capabilities
                Visible, // Visible and occupies space on the overall layout
                Hidden,  // Visibly hidden but still occupies space on the overall layout
                None     // Visibly hidden and doesn't occupiy space on the overall layout
            };
        }
    }
}