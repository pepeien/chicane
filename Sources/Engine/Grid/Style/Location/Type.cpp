#include "Chicane/Grid/Style/Location/Type.hpp"

namespace Chicane
{
    String toString(Grid::StyleLocationType inValue)
    {
        switch (inValue)
        {
        case Grid::StyleLocationType::Local:
            return "Local";

        case Grid::StyleLocationType::URL:
            return "URL";

        default:
            return "";
        }
    }
}
