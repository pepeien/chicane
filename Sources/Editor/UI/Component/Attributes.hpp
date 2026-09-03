#pragma once

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Grid/Component/Container.hpp>

namespace Editor
{
    CH_TYPE(Manual)
    class Attributes : public Chicane::Grid::Container
    {
    public:
        CH_CONSTRUCTOR()
        Attributes(const pugi::xml_node& inNode);
    };
}
