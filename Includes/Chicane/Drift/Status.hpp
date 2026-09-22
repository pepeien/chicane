#pragma once

#include <cstdint>

#include "Chicane/Drift.hpp"
#include "Chicane/Core/String.hpp"

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

    CHICANE_DRIFT String toString(Drift::Status inValue);
}
