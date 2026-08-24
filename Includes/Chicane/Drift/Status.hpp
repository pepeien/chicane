#pragma once

#include <cstdint>

#include "Chicane/Drift.hpp"

namespace Chicane
{
    namespace Drift
    {
        enum class Status : std::uint8_t
        {
            Stopped,
            Playing,
            Paused
        };
    }
}
