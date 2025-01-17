#pragma once

#include "Chicane/Base.hpp"

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