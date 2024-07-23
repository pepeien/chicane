#pragma once

#include "Chicane/Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        typedef std::function<void (const pugi::xml_node&)> ComponentCompiler;

        bool hasComponent(const std::string& inId);
        void addComponent(const std::string& inId, ComponentCompiler inCompiler);
        ComponentCompiler getComponent(const std::string& inId);
    }
}