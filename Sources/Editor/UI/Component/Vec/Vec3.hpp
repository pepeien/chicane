#pragma once

#include <Chicane/Core/Math/Vec/Vec3.hpp>
#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/Xml.hpp>
#include <Chicane/Grid/Component/Container.hpp>

namespace Editor
{
    CH_TYPE(Manual)
    class Vec3 : public Chicane::Grid::Container
    {
    public:
        CH_CONSTRUCTOR()
        Vec3(const pugi::xml_node& inNode);

    public:
        CH_FIELD()
        Chicane::Vec3 value;
    };
}
