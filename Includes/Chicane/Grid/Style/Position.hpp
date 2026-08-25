#pragma once

#include "Chicane/Grid.hpp"

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
}
