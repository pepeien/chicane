#include "Chicane/Grid/Style/Property/Dirty.hpp"

namespace Chicane
{
    String toString(Grid::StylePropertyDirty inValue)
    {
        switch (inValue)
        {
        case Grid::StylePropertyDirty::None:
            return "None";

        case Grid::StylePropertyDirty::Layout:
            return "Layout";

        case Grid::StylePropertyDirty::Paint:
            return "Paint";

        case Grid::StylePropertyDirty::Text:
            return "Text";

        case Grid::StylePropertyDirty::Subtree:
            return "Subtree";

        default:
            return "";
        }
    }
}
