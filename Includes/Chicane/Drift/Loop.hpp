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

        inline String toString(Loop inValue)
        {
            switch (inValue)
            {
            case Loop::Once:
                return "Once";

            case Loop::Repeat:
                return "Repeat";

            case Loop::PingPong:
                return "PingPong";

            default:
                return "";
            }
        }
    }
}
