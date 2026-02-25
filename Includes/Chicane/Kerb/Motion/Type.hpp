#pragma once

#include <cstdint>

namespace Chicane
{
    namespace Kerb
    {
        enum class MotionType : std::uint8_t
        {
            Dynamic,
            Kinematic,
            Static
        };
    }
}