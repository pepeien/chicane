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

    inline String toString(Grid::StyleOverflow inValue)
    {
        switch (inValue)
        {
        case Grid::StyleOverflow::Visible:
            return "Visible";

        case Grid::StyleOverflow::Hidden:
            return "Hidden";

        case Grid::StyleOverflow::Scroll:
            return "Scroll";

        case Grid::StyleOverflow::Auto:
            return "Auto";

        default:
            return "";
        }
    }
}
