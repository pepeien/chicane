#pragma once

#include <cstdint>

#include "Chicane/Drift.hpp"

namespace Chicane
{
    namespace Drift
    {
        enum class Loop : std::uint8_t
        {
            Once,
            Repeat,
            PingPong
        };
    }
}
