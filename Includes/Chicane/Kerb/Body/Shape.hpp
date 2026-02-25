#pragma once

#include <cstdint>

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
}