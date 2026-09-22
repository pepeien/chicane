#include "Chicane/Grid/Component/Svg/Paint/LineJoin.hpp"

namespace Chicane
{
    String toString(Grid::SvgLineJoin inValue)
    {
        switch (inValue)
        {
        case Grid::SvgLineJoin::Miter:
            return "Miter";

        case Grid::SvgLineJoin::Round:
            return "Round";

        case Grid::SvgLineJoin::Bevel:
            return "Bevel";

        default:
            return "";
        }
    }
}
