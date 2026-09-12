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

        inline String toString(StylePosition inValue)
        {
            switch (inValue)
            {
            case StylePosition::Absolute:
                return "Absolute";

            case StylePosition::Relative:
                return "Relative";

            default:
                return "";
            }
        }
    }
}
