#pragma once

#include "Core.hpp"

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