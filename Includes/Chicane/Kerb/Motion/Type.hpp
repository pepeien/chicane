#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

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

    inline String toString(Kerb::MotionType inValue)
    {
        switch (inValue)
        {
        case Kerb::MotionType::Dynamic:
            return "Dynamic";

        case Kerb::MotionType::Kinematic:
            return "Kinematic";

        case Kerb::MotionType::Static:
            return "Static";

        default:
            return "";
        }
    }
}
