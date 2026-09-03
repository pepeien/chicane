#pragma once

#include <vector>

#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleSelectorPart
        {
        public:
            bool                bCanHover = false;
            bool                bCanFocus = false;
            bool                bCanDrag  = false;
            String              tag       = String::empty();
            String              id        = String::empty();
            std::vector<String> classes   = {};
        };
    }
}
