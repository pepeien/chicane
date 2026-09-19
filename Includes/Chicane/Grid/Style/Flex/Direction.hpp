#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class StyleFlexDirection : std::uint8_t
        {
            Column,
            Row
        };
    }

    inline String toString(Grid::StyleFlexDirection inValue)
    {
        switch (inValue)
        {
        case Grid::StyleFlexDirection::Column:
            return "Column";

        case Grid::StyleFlexDirection::Row:
            return "Row";

        default:
            return "";
        }
    }
}
