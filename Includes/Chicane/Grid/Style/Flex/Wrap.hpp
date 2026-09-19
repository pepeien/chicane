#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class StyleFlexWrap : std::uint8_t
        {
            NoWrap,
            Wrap
        };
    }

    inline String toString(Grid::StyleFlexWrap inValue)
    {
        switch (inValue)
        {
        case Grid::StyleFlexWrap::NoWrap:
            return "NoWrap";

        case Grid::StyleFlexWrap::Wrap:
            return "Wrap";

        default:
            return "";
        }
    }
}
