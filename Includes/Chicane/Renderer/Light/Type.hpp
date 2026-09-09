#pragma once

#include <cstdint>

#include "Chicane/Core/Reflection.hpp"

namespace Chicane
{
    CH_ENUM()
    enum class LightType : std::uint8_t
    {
        Directional,
        Point,
        Spot,
        Environment
    };
}
