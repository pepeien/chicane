#include "Chicane/Screech/Device/Type.hpp"

namespace Chicane
{
    String toString(Screech::DeviceType inValue)
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
