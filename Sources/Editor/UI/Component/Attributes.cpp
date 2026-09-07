#include "Editor/UI/Component/Attributes.reflected.hpp"

#include "Editor/UI/Component/Dock/Header.hpp"
#include "Editor/UI/Component/Attributes/Tab.hpp"
#include "Editor/UI/Component/Vec/Vec3.hpp"

namespace Editor
{
    Attributes::Attributes(const pugi::xml_node& inNode)
        : Chicane::Grid::Container(inNode)
    {
        import <AttributesTab>();
        import <DockHeader>();
        import <Vec3>();

        load("Assets/Editor/UI/Components/Attributes.grid", "Assets/Editor/UI/Components/Attributes.decal");
    }
}
