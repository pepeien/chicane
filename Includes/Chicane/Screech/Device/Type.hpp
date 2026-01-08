#pragma once

#include "Chicane/Screech.hpp"

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
}