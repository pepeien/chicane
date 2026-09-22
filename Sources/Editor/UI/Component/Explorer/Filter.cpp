#include "Editor/UI/Component/Explorer/Filter.hpp"

namespace Editor
{
    Chicane::String toString(ExplorerFilter inValue)
    {
        switch (inValue)
        {
        case ExplorerFilter::All:
            return "All";

        case ExplorerFilter::Folders:
            return "Folders";

        case ExplorerFilter::Files:
            return "Files";

        default:
            return "";
        }
    }
}
