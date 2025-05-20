#pragma once

#include "Chicane/Grid/Essential.hpp"
#include "Chicane/Grid/Component.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Loader
        {
            CHICANE_GRID Component* createComponent(const pugi::xml_node& inNode);
            CHICANE_GRID void registerComponent(const std::string& inTag, Component::Compiler inCompiler);
        }
    }
}