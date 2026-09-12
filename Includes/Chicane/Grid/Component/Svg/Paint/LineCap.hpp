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

        inline String toString(SvgLineCap inValue)
        {
            switch (inValue)
            {
            case SvgLineCap::Butt:
                return "Butt";

            case SvgLineCap::Round:
                return "Round";

            case SvgLineCap::Square:
                return "Square";

            default:
                return "";
            }
        }
    }
}
