#pragma once

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Grid/Component/Container.hpp>

namespace Editor
{
    CH_TYPE(Type = (Manual), Alias = (Editor::Outliner))
    class Outliner : public Chicane::Grid::Container
    {
    public:
        CH_CONSTRUCTOR()
        Outliner(const pugi::xml_node& inNode);
    };
}
