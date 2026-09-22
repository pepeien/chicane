#include "Chicane/Screech/Sound/Status.hpp"

namespace Chicane
{
    String toString(Screech::SoundStatus inValue)
    {
        switch (inValue)
        {
        case Screech::SoundStatus::Playing:
            return "Playing";

        case Screech::SoundStatus::Paused:
            return "Paused";

        case Screech::SoundStatus::Stopped:
            return "Stopped";

        default:
            return "";
        }
    }
}
