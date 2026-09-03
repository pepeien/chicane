#include "Editor/UI/Component/Outliner.reflected.hpp"

#include "Editor/UI/Component/Dock/Header.hpp"

namespace Editor
{
    Outliner::Outliner(const pugi::xml_node& inNode)
        : Chicane::Grid::Container(inNode)
    {
        import <DockHeader>();

        load("Assets/Editor/UI/Components/Outliner.grid", "Assets/Editor/UI/Components/Outliner.decal");
    }
}
