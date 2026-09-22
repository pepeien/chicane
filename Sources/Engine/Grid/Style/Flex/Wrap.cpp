#include "Chicane/Grid/Style/Flex/Wrap.hpp"

namespace Chicane
{
    String toString(Grid::StyleFlexWrap inValue)
    {
        switch (inValue)
        {
        case Grid::StyleFlexWrap::NoWrap:
            return "NoWrap";

        case Grid::StyleFlexWrap::Wrap:
            return "Wrap";

        default:
            return "";
        }
    }
}
