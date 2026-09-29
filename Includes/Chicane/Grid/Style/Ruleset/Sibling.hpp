#pragma once

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleSiblingSelector
        {
        public:
            bool bOfType  = false;
            bool bFromEnd = false;
            int  step     = 0;
            int  offset   = 1;
        };
    }
}
