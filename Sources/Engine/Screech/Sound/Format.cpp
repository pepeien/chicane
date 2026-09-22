#include "Chicane/Screech/Sound/Format.hpp"

namespace Chicane
{
    String toString(Screech::SoundFormat inValue)
    {
        switch (inValue)
        {
        case Screech::SoundFormat::Unknown:
            return "Unknown";

        case Screech::SoundFormat::U8:
            return "U8";

        case Screech::SoundFormat::S16:
            return "S16";

        case Screech::SoundFormat::S24:
            return "S24";

        case Screech::SoundFormat::S32:
            return "S32";

        case Screech::SoundFormat::F32:
            return "F32";

        case Screech::SoundFormat::Count:
            return "Count";

        default:
            return "";
        }
    }
}
