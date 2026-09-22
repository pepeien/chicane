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
    }

    CHICANE_SCREECH String toString(Screech::SoundStatus inValue);
}
