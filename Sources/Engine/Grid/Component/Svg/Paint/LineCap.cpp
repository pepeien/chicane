#include "Chicane/Grid/Component/Svg/Paint/LineCap.hpp"

namespace Chicane
{
    String toString(Grid::SvgLineCap inValue)
    {
        switch (inValue)
        {
        case Grid::SvgLineCap::Butt:
            return "Butt";

        case Grid::SvgLineCap::Round:
            return "Round";

        case Grid::SvgLineCap::Square:
            return "Square";

        default:
            return "";
        }
    }
}
