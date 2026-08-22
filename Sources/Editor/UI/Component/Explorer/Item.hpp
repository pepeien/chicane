#pragma once

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/Xml.hpp>
#include <Chicane/Grid/Component/Button.hpp>

namespace Editor
{
    CH_TYPE(Manual)
    class ExplorerItem : public Chicane::Grid::Button
    {
    public:
        CH_CONSTRUCTOR()
        ExplorerItem(const pugi::xml_node& inNode);
    };
}
