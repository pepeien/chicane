#pragma once

#include <cstdint>
#include <vector>

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Style/Ruleset/Part.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleCompiledSelector
        {
        public:
            std::uint32_t specificity() const;

        public:
            std::vector<StyleSelectorPart> chain = {};
        };
    }
}
