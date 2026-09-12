#pragma once

#include <cstdint>

#include "Chicane/Drift.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Drift
    {
        enum class Direction : std::uint8_t
        {
            Forward,
            Reverse
        };

        inline String toString(Direction inValue)
        {
            switch (inValue)
            {
            case Direction::Forward:
                return "Forward";

            case Direction::Reverse:
                return "Reverse";

            default:
                return "";
            }
        }
    }
}
