#pragma once

#include <cstdint>

#include "Chicane/Drift.hpp"
#include "Chicane/Core/String.hpp"

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

    CHICANE_DRIFT String toString(Drift::Loop inValue);
}
