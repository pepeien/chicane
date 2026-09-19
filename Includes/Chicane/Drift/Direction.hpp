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
    }

    inline String toString(Drift::Direction inValue)
    {
        switch (inValue)
        {
        case Drift::Direction::Forward:
            return "Forward";

        case Drift::Direction::Reverse:
            return "Reverse";

        default:
            return "";
        }
    }
}
