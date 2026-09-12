#pragma once

#include <cstdint>

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"

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

    inline String toString(LightType inValue)
    {
        switch (inValue)
        {
        case LightType::Directional:
            return "Directional";

        case LightType::Point:
            return "Point";

        case LightType::Spot:
            return "Spot";

        case LightType::Environment:
            return "Environment";

        default:
            return "";
        }
    }
}
