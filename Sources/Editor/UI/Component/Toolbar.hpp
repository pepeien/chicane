#pragma once

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>
#include <Chicane/Core/Xml.hpp>
#include <Chicane/Grid/Component/Container.hpp>

namespace Editor
{
    CH_TYPE(Type = (Manual), Alias = (Editor::Toolbar))
    class Toolbar : public Chicane::Grid::Container
    {
    public:
        CH_CONSTRUCTOR()
        Toolbar(const pugi::xml_node& inNode);
    };
}
