#include "Chicane/Screech/Sound/Flags.hpp"

namespace Chicane
{
    String toString(Screech::SoundFlag inValue)
    {
        switch (inValue)
        {
        case Screech::SoundFlag::Stream:
            return "Stream";

        case Screech::SoundFlag::Decode:
            return "Decode";

        case Screech::SoundFlag::Async:
            return "Async";

        case Screech::SoundFlag::WaitInit:
            return "WaitInit";

        case Screech::SoundFlag::UnknownLength:
            return "UnknownLength";

        case Screech::SoundFlag::Looping:
            return "Looping";

        case Screech::SoundFlag::DefaultAttachment:
            return "DefaultAttachment";

        case Screech::SoundFlag::NoPitch:
            return "NoPitch";

        case Screech::SoundFlag::NoSpatialization:
            return "NoSpatialization";

        default:
            return "";
        }
    }
}
