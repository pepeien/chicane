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

    inline String toString(Box::AnimationLoop inValue)
    {
        switch (inValue)
        {
        case Box::AnimationLoop::Once:
            return "Once";

        case Box::AnimationLoop::Repeat:
            return "Repeat";

        case Box::AnimationLoop::PingPong:
            return "PingPong";

        default:
            return "";
        }
    }
}
