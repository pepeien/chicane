#include "Chicane/Grid/Component/Dock/Side.hpp"

namespace Chicane
{
    String toString(Grid::DockSide inValue)
    {
        switch (inValue)
        {
        case Grid::DockSide::Fill:
            return "Fill";

        case Grid::DockSide::Float:
            return "Float";

        case Grid::DockSide::Left:
            return "Left";

        case Grid::DockSide::Right:
            return "Right";

        case Grid::DockSide::Top:
            return "Top";

        case Grid::DockSide::Bottom:
            return "Bottom";

        default:
            return "";
        }
    }
}
