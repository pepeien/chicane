#pragma once

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Grid/Component.hpp>

namespace Editor
{
    CH_TYPE(Manual)
    class Outliner : public Chicane::Grid::Component
    {
    public:
        CH_CONSTRUCTOR()
        Outliner(const pugi::xml_node& inNode);
    };
}
