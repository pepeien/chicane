#pragma once

#include <cstdint>

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct StyleRuleset;

        struct CHICANE_GRID StyleMatch
        {
        public:
            std::uint32_t       origin      = 0;
            std::uint32_t       specificity = 0;
            std::uint32_t       order       = 0;
            const StyleRuleset* source      = nullptr;
        };
    }
}
