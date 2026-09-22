#include "Chicane/Grid/Component/Status.hpp"

namespace Chicane
{
    String toString(Grid::ComponentStatus inValue)
    {
        switch (inValue)
        {
        case Grid::ComponentStatus::None:
            return "None";

        case Grid::ComponentStatus::Hovered:
            return "Hovered";

        case Grid::ComponentStatus::Focused:
            return "Focused";

        case Grid::ComponentStatus::Dragging:
            return "Dragging";

        case Grid::ComponentStatus::Culled:
            return "Culled";

        default:
            return "";
        }
    }
}
