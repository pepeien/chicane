#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"
#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class StyleWordBreak : std::uint8_t
        {
            Normal,    // Break only on explicit newlines
            BreakWord, // Wrap at word boundaries, then characters if needed
            BreakAll   // Wrap at any character
        };
    }

    CHICANE_GRID String toString(Grid::StyleWordBreak inValue);
}
