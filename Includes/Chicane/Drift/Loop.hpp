#pragma once

#include <cstdint>

#include "Chicane/Drift.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Drift
    {
        enum class Loop : std::uint8_t
        {
            Once,
            Repeat,
            PingPong
        };
    }

    inline String toString(Drift::Loop inValue)
    {
        switch (inValue)
        {
        case Drift::Loop::Once:
            return "Once";

        case Drift::Loop::Repeat:
            return "Repeat";

        case Drift::Loop::PingPong:
            return "PingPong";

        default:
            return "";
        }
    }
}
