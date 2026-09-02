#pragma once

#include "Chicane/Core/Math/Bounds/2D.hpp"

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID DockRegion
        {
        public:
            Bounds2D box          = {};
            Bounds2D splitter     = {};
            bool     bHasSplitter = false;
        };
    }
}
