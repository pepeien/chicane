#pragma once

#include <cstdint>

#include "Chicane/Screech.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Screech
    {
        enum class DeviceType : std::uint8_t
        {
            Playback = 1,
            Capture  = 2,
            Duplex   = Playback | Capture,
            Loopback = 4,
            Unknown
        };
    }

    CHICANE_SCREECH String toString(Screech::DeviceType inValue);
}
