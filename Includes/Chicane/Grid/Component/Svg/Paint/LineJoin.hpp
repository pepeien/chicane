#pragma once

#include <cstdint>

#include "Chicane/Grid.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class SvgLineJoin : std::uint8_t
        {
            Miter,
            Round,
            Bevel
        };
    }

    inline String toString(Grid::SvgLineJoin inValue)
    {
        switch (inValue)
        {
        case Grid::SvgLineJoin::Miter:
            return "Miter";

        case Grid::SvgLineJoin::Round:
            return "Round";

        case Grid::SvgLineJoin::Bevel:
            return "Bevel";

        default:
            return "";
        }
    }
}
