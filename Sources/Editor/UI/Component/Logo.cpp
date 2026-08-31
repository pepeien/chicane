#include "Editor/UI/Component/Logo.reflected.hpp"

namespace Editor
{
    Logo::Logo(const pugi::xml_node& inNode)
        : Chicane::Grid::Component(inNode)
    {
        load("Assets/Editor/UI/Components/Logo.grid", "Assets/Editor/UI/Components/Logo.decal");
    }
}