#include "Editor/UI/Component/Explorer/Item/Kind.hpp"
#include "Editor/UI/Component/Explorer/Item/Kind.reflected.hpp"

namespace Editor
{
    Chicane::String toString(ExplorerItemKind inValue)
    {
        switch (inValue)
        {
        case ExplorerItemKind::Folder:
            return "Folder";

        case ExplorerItemKind::File:
            return "File";

        case ExplorerItemKind::Preview:
            return "Preview";

        default:
            return "";
        }
    }
}
