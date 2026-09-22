#include "Chicane/Grid/Component/Dirty.hpp"

namespace Chicane
{
    String toString(Grid::ComponentDirty inValue)
    {
        switch (inValue)
        {
        case Grid::ComponentDirty::None:
            return "None";

        case Grid::ComponentDirty::Style:
            return "Style";

        case Grid::ComponentDirty::Layout:
            return "Layout";

        case Grid::ComponentDirty::Paint:
            return "Paint";

        case Grid::ComponentDirty::Text:
            return "Text";

        case Grid::ComponentDirty::LaidOut:
            return "LaidOut";

        case Grid::ComponentDirty::Insets:
            return "Insets";

        case Grid::ComponentDirty::LiveBind:
            return "LiveBind";

        default:
            return "";
        }
    }
}
