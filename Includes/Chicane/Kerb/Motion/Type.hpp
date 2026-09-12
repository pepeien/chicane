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

        inline String toString(MotionType inValue)
        {
            switch (inValue)
            {
            case MotionType::Dynamic:
                return "Dynamic";

            case MotionType::Kinematic:
                return "Kinematic";

            case MotionType::Static:
                return "Static";

            default:
                return "";
            }
        }
    }
}