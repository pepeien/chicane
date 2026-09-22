#pragma once

#include <cstdint>

#include "Chicane/Grid.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class StyleOverflow : std::uint8_t
        {
            Visible, // Content may paint outside the box
            Hidden,  // Clip overflowing content
            Scroll,  // Clip and allow scrolling
            Auto     // Clip and scroll only when content overflows
        };
    }

    CHICANE_GRID String toString(Grid::StyleOverflow inValue);
}
