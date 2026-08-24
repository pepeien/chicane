#pragma once

#include <cstdint>

#include "Chicane/Drift.hpp"

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
}
