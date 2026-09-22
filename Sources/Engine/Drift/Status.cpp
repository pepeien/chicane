#include "Chicane/Drift/Status.hpp"

namespace Chicane
{
    String toString(Drift::Status inValue)
    {
        switch (inValue)
        {
        case Drift::Status::Stopped:
            return "Stopped";

        case Drift::Status::Playing:
            return "Playing";

        case Drift::Status::Paused:
            return "Paused";

        default:
            return "";
        }
    }
}
