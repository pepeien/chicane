#include "Chicane/Grid/Style/Border/Type.hpp"

namespace Chicane
{
    String toString(Grid::StyleBorderType inValue)
    {
        switch (inValue)
        {
        case Grid::StyleBorderType::None:
            return "None";

        case Grid::StyleBorderType::Solid:
            return "Solid";

        default:
            return "";
        }
    }
}
