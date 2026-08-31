#pragma once

#include <cstdint>

#include "Chicane/Grid.hpp"

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
}
