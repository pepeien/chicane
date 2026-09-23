#pragma once

#include <cstdint>

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Kerb.hpp"

namespace Chicane
{
    namespace Kerb
    {
        CH_ENUM()
        enum class MotionType : std::uint8_t
        {
            Dynamic,
            Kinematic,
            Static
        };
    }

    CHICANE_KERB String toString(Kerb::MotionType inValue);
}
