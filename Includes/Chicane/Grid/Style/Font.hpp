#pragma once

#include "Chicane/Core/String.hpp"
#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleFont
        {
        public:
            StyleFont();

        public:
            String family;
            float  size;
        };
    }
}