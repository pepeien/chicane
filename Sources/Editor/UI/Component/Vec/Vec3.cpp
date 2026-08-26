#include "Editor/UI/Component/Vec/Vec3.reflected.hpp"

namespace Editor
{
    Vec3::Vec3(const pugi::xml_node& inNode)
        : Chicane::Grid::Container(inNode),
          value(Chicane::Vec3::One())
    {
        load("Assets/Editor/UI/Components/Vec/Vec3.grid", "Assets/Editor/UI/Components/Vec/Vec3.decal");
    }
}
