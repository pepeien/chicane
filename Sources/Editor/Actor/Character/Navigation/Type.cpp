#include "Editor/Actor/Character/Navigation/Type.hpp"
#include "Editor/Actor/Character/Navigation/Type.reflected.hpp"

namespace Editor
{
    Chicane::String toString(NavigationType inValue)
    {
        switch (inValue)
        {
        case NavigationType::None:
            return "None";

        case NavigationType::Orbit:
            return "Orbit";

        case NavigationType::Pan:
            return "Pan";

        case NavigationType::Zoom:
            return "Zoom";

        default:
            return "";
        }
    }
}
