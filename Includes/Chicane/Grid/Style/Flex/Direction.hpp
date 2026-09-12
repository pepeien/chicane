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

        inline String toString(StyleFlexDirection inValue)
        {
            switch (inValue)
            {
            case StyleFlexDirection::Column:
                return "Column";

            case StyleFlexDirection::Row:
                return "Row";

            default:
                return "";
            }
        }
    }
}