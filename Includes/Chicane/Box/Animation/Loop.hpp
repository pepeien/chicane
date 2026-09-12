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

        inline String toString(AnimationLoop inValue)
        {
            switch (inValue)
            {
            case AnimationLoop::Once:
                return "Once";

            case AnimationLoop::Repeat:
                return "Repeat";

            case AnimationLoop::PingPong:
                return "PingPong";

            default:
                return "";
            }
        }
    }
}
