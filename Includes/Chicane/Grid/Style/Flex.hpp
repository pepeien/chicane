#pragma once

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Style/Flex/Direction.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleFlex
        {
        public:
            StyleFlexDirection direction = StyleFlexDirection::Row;
        };
    }
}