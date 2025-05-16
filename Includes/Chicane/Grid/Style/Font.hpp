#pragma once

#include "Chicane/Grid/Style/Essential.hpp"
#include "Chicane/Grid/Style/Source.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            struct CHICANE_GRID Font
            {
            public:
                void update(const Properties &inSource);

            public:
                std::string family = FONT_FAMILY_DEFAULT_VALUE;
                std::string size   = FONT_SIZE_DEFAULT_VALUE;
            };
        }
    }
}