#include "Editor/UI/Component/Logo.reflected.hpp"

namespace Editor
{
    Logo::Logo(const Chicane::XmlNode& inNode)
        : Chicane::Grid::Component(inNode)
    {
        load("Assets/Editor/UI/Components/Logo.grid", "Assets/Editor/UI/Components/Logo.decal");
    }
}