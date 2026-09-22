#include "Chicane/Grid/Style/Overflow.hpp"

namespace Chicane
{
    String toString(Grid::StyleOverflow inValue)
    {
        switch (inValue)
        {
        case Grid::StyleOverflow::Visible:
            return "Visible";

        case Grid::StyleOverflow::Hidden:
            return "Hidden";

        case Grid::StyleOverflow::Scroll:
            return "Scroll";

        case Grid::StyleOverflow::Auto:
            return "Auto";

        default:
            return "";
        }
    }
}
