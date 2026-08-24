#include "Editor/UI/Component/Outliner.reflected.hpp"

namespace Editor
{
    Outliner::Outliner(const pugi::xml_node& inNode)
        : Chicane::Grid::Component(inNode)
    {
        load("Assets/Editor/UI/Components/Outliner.grid", "Assets/Editor/UI/Components/Outliner.decal");
    }
}
