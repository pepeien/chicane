#include "Editor/UI/Component/Outliner.reflected.hpp"

#include "Editor/UI/Component/Outliner/Tab.hpp"

namespace Editor
{
    Outliner::Outliner(const pugi::xml_node& inNode)
        : Chicane::Grid::Component(inNode)
    {
        import <OutlinerTab>();

        load("Assets/Editor/UI/Components/Outliner.grid", "Assets/Editor/UI/Components/Outliner.decal");
    }
}
