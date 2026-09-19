#pragma once

#include "Chicane/Grid.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class StylePosition : std::uint8_t
        {
            Absolute, // Out of flow; origin is the nearest positioned ancestor, else the root
            Relative  // In flow; also a containing block when explicitly set
        };
    }

    inline String toString(Grid::StylePosition inValue)
    {
        switch (inValue)
        {
        case Grid::StylePosition::Absolute:
            return "Absolute";

        case Grid::StylePosition::Relative:
            return "Relative";

        default:
            return "";
        }
    }
}
