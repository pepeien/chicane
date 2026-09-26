#pragma once

#include <cstdint>
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
            std::uint32_t specificity() const;

        public:
            bool                bCanHover = false;
            bool                bCanFocus = false;
            bool                bCanDrag  = false;
            String              tag       = String::sEmpty();
            String              id        = String::sEmpty();
            std::vector<String> classes   = {};
        };
    }
}
