#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"
#include "Chicane/Kerb.hpp"

namespace Chicane
{
    namespace Kerb
    {
        enum class BodyShape : std::uint8_t
        {
            Box,
            Capsule,
            Polygon
        };
    }

    CHICANE_KERB String toString(Kerb::BodyShape inValue);
}
