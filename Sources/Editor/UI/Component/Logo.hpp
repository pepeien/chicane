#pragma once

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Grid/Component.hpp>

namespace Editor
{
    CH_TYPE(Manual)
    class Logo : public Chicane::Grid::Component
    {
    public:
        CH_CONSTRUCTOR()
        Logo(const pugi::xml_node& inNode);
    };
}