#pragma once

#include <cstdint>

#include "Chicane/Grid.hpp"

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
}
