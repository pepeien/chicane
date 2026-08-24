#pragma once

#include <vector>

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Style/Ruleset.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleKeyframe
        {
        public:
            using List = std::vector<StyleKeyframe>;

        public:
            StyleKeyframe();

        public:
            float                    offset;
            StyleRuleset::Properties properties;
        };
    }
}
