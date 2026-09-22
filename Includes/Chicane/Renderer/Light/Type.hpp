#pragma once

#include <cstdint>

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Renderer.hpp"

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

    CHICANE_RENDERER String toString(LightType inValue);
}
