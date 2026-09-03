#include "Editor/UI/Component/Attributes.reflected.hpp"

#include "Editor/UI/Component/Dock/Header.hpp"
#include "Editor/UI/Component/Outliner/Tab.hpp"
#include "Editor/UI/Component/Vec/Vec3.hpp"

namespace Editor
{
    Attributes::Attributes(const pugi::xml_node& inNode)
        : Chicane::Grid::Container(inNode)
    {
        import <DockHeader>();
        import <OutlinerTab>();
        import <Vec3>();

        load("Assets/Editor/UI/Components/Attributes.grid", "Assets/Editor/UI/Components/Attributes.decal");
    }
}
