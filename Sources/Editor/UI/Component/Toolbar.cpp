#include "Editor/UI/Component/Toolbar.reflected.hpp"

namespace Editor
{
    Toolbar::Toolbar(const pugi::xml_node& inNode)
        : Chicane::Grid::Container(inNode)
    {
        load("Assets/Editor/UI/Components/Toolbar.grid", "Assets/Editor/UI/Components/Toolbar.decal");
    }
}
