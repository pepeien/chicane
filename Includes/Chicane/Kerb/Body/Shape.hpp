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

        inline String toString(BodyShape inValue)
        {
            switch (inValue)
            {
            case BodyShape::Box:
                return "Box";

            case BodyShape::Capsule:
                return "Capsule";

            case BodyShape::Polygon:
                return "Polygon";

            default:
                return "";
            }
        }
    }
}