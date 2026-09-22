#include "Chicane/Renderer/Light/Type.hpp"
#include "Chicane/Renderer/Light/Type.reflected.hpp"

namespace Chicane
{
    String toString(LightType inValue)
    {
        switch (inValue)
        {
        case LightType::Directional:
            return "Directional";

        case LightType::Point:
            return "Point";

        case LightType::Spot:
            return "Spot";

        case LightType::Environment:
            return "Environment";

        default:
            return "";
        }
    }
}
