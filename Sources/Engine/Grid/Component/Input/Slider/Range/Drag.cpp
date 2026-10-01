#include "Chicane/Grid/Component/Input/Slider/Range/Drag.hpp"

namespace Chicane
{
    String toString(Grid::InputRangeSliderDrag inValue)
    {
        switch (inValue)
        {
        case Grid::InputRangeSliderDrag::None:
            return "None";

        case Grid::InputRangeSliderDrag::Low:
            return "Low";

        case Grid::InputRangeSliderDrag::High:
            return "High";

        case Grid::InputRangeSliderDrag::Span:
            return "Span";

        default:
            return "";
        }
    }
}
