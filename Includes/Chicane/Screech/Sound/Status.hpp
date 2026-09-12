#pragma once

#include <cstdint>

#include "Chicane/Screech.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Screech
    {
        enum class SoundStatus : std::uint8_t
        {
            Playing,
            Paused,
            Stopped
        };

        inline String toString(SoundStatus inValue)
        {
            switch (inValue)
            {
            case SoundStatus::Playing:
                return "Playing";

            case SoundStatus::Paused:
                return "Paused";

            case SoundStatus::Stopped:
                return "Stopped";

            default:
                return "";
            }
        }
    }
}