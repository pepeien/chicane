#pragma once

#include "Chicane/Screech/Essential.hpp"

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
}