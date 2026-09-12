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

        inline String toString(SvgLineJoin inValue)
        {
            switch (inValue)
            {
            case SvgLineJoin::Miter:
                return "Miter";

            case SvgLineJoin::Round:
                return "Round";

            case SvgLineJoin::Bevel:
                return "Bevel";

            default:
                return "";
            }
        }
    }
}
