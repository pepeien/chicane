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

    inline String toString(Screech::DeviceType inValue)
    {
        switch (inValue)
        {
        case Screech::DeviceType::Playback:
            return "Playback";

        case Screech::DeviceType::Capture:
            return "Capture";

        case Screech::DeviceType::Duplex:
            return "Duplex";

        case Screech::DeviceType::Loopback:
            return "Loopback";

        case Screech::DeviceType::Unknown:
            return "Unknown";

        default:
            return "";
        }
    }
}
