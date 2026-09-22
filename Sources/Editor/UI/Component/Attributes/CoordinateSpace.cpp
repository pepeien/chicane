#include "Editor/UI/Component/Attributes/CoordinateSpace.hpp"

namespace Editor
{
    Chicane::String toString(CoordinateSpace inValue)
    {
        switch (inValue)
        {
        case CoordinateSpace::Relative:
            return "Relative";

        case CoordinateSpace::Absolute:
        default:
            return "Absolute";
        }
    }
}
