#include "Chicane/Grid/Style/Position.hpp"

namespace Chicane
{
    String toString(Grid::StylePosition inValue)
    {
        switch (inValue)
        {
        case Grid::StylePosition::Absolute:
            return "Absolute";

        case Grid::StylePosition::Relative:
            return "Relative";

        default:
            return "";
        }
    }
}
