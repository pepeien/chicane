#include "Editor/UI/Component/Explorer/Item.reflected.hpp"

namespace Editor
{
    ExplorerItem::ExplorerItem(const pugi::xml_node& inNode)
        : Chicane::Grid::Button(inNode)
    {
        load("Assets/Editor/UI/Components/Explorer/Item.grid", "Assets/Editor/UI/Components/Explorer/Item.decal");
    }
}
