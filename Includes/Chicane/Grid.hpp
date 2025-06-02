#pragma once

#include "Chicane/Grid/Component.hpp"
#include "Chicane/Grid/Essential.hpp"
#include "Chicane/Grid/Event.hpp"
#include "Chicane/Grid/Function.hpp"
#include "Chicane/Grid/Primitive.hpp"
#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        CHICANE_GRID Component* createComponent(const pugi::xml_node& inNode);
        CHICANE_GRID void registerComponent(const std::string& inTag, Component::Compiler inCompiler);
    }
}