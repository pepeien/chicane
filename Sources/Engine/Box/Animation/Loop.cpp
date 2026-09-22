#include "Chicane/Box/Animation/Loop.hpp"

namespace Chicane
{
    String toString(Box::AnimationLoop inValue)
    {
        switch (inValue)
        {
        case Box::AnimationLoop::Once:
            return "Once";

        case Box::AnimationLoop::Repeat:
            return "Repeat";

        case Box::AnimationLoop::PingPong:
            return "PingPong";

        default:
            return "";
        }
    }
}
