#include "Chicane/Grid/Style/Flex/Direction.hpp"

namespace Chicane
{
    String toString(Grid::StyleFlexDirection inValue)
    {
        switch (inValue)
        {
        case Grid::StyleFlexDirection::Column:
            return "Column";

        case Grid::StyleFlexDirection::Row:
            return "Row";

        default:
            return "";
        }
    }
}
