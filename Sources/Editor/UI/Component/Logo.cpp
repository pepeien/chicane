#include "Editor/UI/Component/Logo.reflected.hpp"

namespace Editor
{
    Logo::Logo(const Chicane::XmlNode& inNode)
        : Chicane::Grid::Component(inNode)
    {
        load("Assets/Editor/UI/Components/Logo/Index.grid", "Assets/Editor/UI/Components/Logo/Index.decal");
    }
}