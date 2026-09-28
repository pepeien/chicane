#pragma once

#include <cstdint>
#include <vector>

#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component/Status.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleSelectorPart
        {
        public:
            std::uint32_t specificity() const;

        public:
            ComponentStatus     status  = ComponentStatus::None;
            String              tag     = String::sEmpty();
            String              id      = String::sEmpty();
            std::vector<String> classes = {};
        };
    }
}
