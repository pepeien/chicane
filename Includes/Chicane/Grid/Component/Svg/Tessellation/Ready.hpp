#pragma once

#include <string>

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Primitive.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID SvgTessellationReady
        {
            std::string key;
            Primitive   primitive;
        };
    }
}
