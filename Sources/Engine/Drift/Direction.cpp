#include "Chicane/Drift/Direction.hpp"

namespace Chicane
{
    String toString(Drift::Direction inValue)
    {
        switch (inValue)
        {
        case Drift::Direction::Forward:
            return "Forward";

        case Drift::Direction::Reverse:
            return "Reverse";

        default:
            return "";
        }
    }
}
