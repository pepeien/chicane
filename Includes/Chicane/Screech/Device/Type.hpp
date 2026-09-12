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

        inline String toString(DeviceType inValue)
        {
            switch (inValue)
            {
            case DeviceType::Playback:
                return "Playback";

            case DeviceType::Capture:
                return "Capture";

            case DeviceType::Duplex:
                return "Duplex";

            case DeviceType::Loopback:
                return "Loopback";

            case DeviceType::Unknown:
                return "Unknown";

            default:
                return "";
            }
        }
    }
}