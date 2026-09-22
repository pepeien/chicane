#include "Chicane/Drift/Loop.hpp"

namespace Chicane
{
    String toString(Drift::Loop inValue)
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
