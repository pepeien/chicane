#pragma once

#include "Chicane.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Component
        {
            namespace Compiler
            {
                typedef std::function<void (const pugi::xml_node&)> Function;
            }
        }
    }
}