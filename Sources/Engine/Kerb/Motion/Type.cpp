#include "Chicane/Kerb/Motion/Type.hpp"

namespace Chicane
{
    String toString(Kerb::MotionType inValue)
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
