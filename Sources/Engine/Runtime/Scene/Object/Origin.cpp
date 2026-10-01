#include "Chicane/Runtime/Scene/Object/Origin.hpp"

namespace Chicane
{
    String toString(ObjectOrigin inValue)
    {
        switch (inValue)
        {
        case ObjectOrigin::Native:
            return "Native";

        case ObjectOrigin::Instance:
            return "Instance";

        case ObjectOrigin::Spawned:
            return "Spawned";

        case ObjectOrigin::Transient:
            return "Transient";

        default:
            return "";
        }
    }
}
