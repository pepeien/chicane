#include "Chicane/Grid/Style/Display.hpp"

namespace Chicane
{
    String toString(Grid::StyleDisplay inValue)
    {
        switch (inValue)
        {
        case Grid::StyleDisplay::Flex:
            return "Flex";

        case Grid::StyleDisplay::Block:
            return "Block";

        case Grid::StyleDisplay::Hidden:
            return "Hidden";

        case Grid::StyleDisplay::None:
            return "None";

        default:
            return "";
        }
    }
}
