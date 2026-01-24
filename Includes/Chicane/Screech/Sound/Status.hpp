#pragma once

#include <cstdint>

#include "Chicane/Screech.hpp"

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