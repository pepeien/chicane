#pragma once

#include <cstdint>

#include "Chicane/Grid.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class SvgLineCap : std::uint8_t
        {
            Butt,
            Round,
            Square
        };
    }

    inline String toString(Grid::SvgLineCap inValue)
    {
        switch (inValue)
        {
        case Grid::SvgLineCap::Butt:
            return "Butt";

        case Grid::SvgLineCap::Round:
            return "Round";

        case Grid::SvgLineCap::Square:
            return "Square";

        default:
            return "";
        }
    }
}
