#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

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

    inline String toString(Kerb::BodyShape inValue)
    {
        switch (inValue)
        {
        case Kerb::BodyShape::Box:
            return "Box";

        case Kerb::BodyShape::Capsule:
            return "Capsule";

        case Kerb::BodyShape::Polygon:
            return "Polygon";

        default:
            return "";
        }
    }
}
