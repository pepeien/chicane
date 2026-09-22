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

    CHICANE_DRIFT String toString(Drift::Direction inValue);
}
