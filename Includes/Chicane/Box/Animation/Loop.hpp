#pragma once

#include <cstdint>

#include "Chicane/Box.hpp"

namespace Chicane
{
    namespace Box
    {
        enum class AnimationLoop : std::uint8_t
        {
            Once,
            Repeat,
            PingPong
        };
    }
}
