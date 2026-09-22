#include "Chicane/Kerb/Body/Shape.hpp"

namespace Chicane
{
    String toString(Kerb::BodyShape inValue)
    {
        switch (inValue)
        {
        case Kerb::BodyShape::Box:
            return "Box";

        case Kerb::BodyShape::Capsule:
            return "Capsule";

        case Kerb::BodyShape::Polygon:
            return "Polygon";

        default:
            return "";
        }
    }
}
