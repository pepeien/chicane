#include "Chicane/Grid/Style/Background/Gradient/Type.hpp"

namespace Chicane
{
    String toString(Grid::StyleGradientType inValue)
    {
        switch (inValue)
        {
        case Grid::StyleGradientType::Linear:
            return "Linear";

        case Grid::StyleGradientType::Radial:
            return "Radial";

        default:
            return "None";
        }
    }
}
