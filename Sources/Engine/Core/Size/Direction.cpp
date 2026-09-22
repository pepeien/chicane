#include "Chicane/Core/Size/Direction.hpp"

namespace Chicane
{
    String toString(SizeDirection inValue)
    {
        switch (inValue)
        {
        case SizeDirection::Vertical:
            return "Vertical";

        case SizeDirection::Horizontal:
            return "Horizontal";

        default:
            return "";
        }
    }
}
