#pragma once

#include <cstdint>
#include <vector>

#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component/Status.hpp"
#include "Chicane/Grid/Style/Ruleset/Sibling.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleSelectorPart
        {
        public:
            std::uint32_t specificity() const;

        public:
            ComponentStatus                   status   = ComponentStatus::None;
            std::vector<StyleSiblingSelector> siblings = {};
            String                            tag      = String::sEmpty();
            String                            id       = String::sEmpty();
            std::vector<String>               classes  = {};
        };
    }
}
