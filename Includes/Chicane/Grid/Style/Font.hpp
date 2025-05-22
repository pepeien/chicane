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
                std::string family = FONT_FAMILY_DEFAULT_VALUE;
                float size         = 16.0f;
            };
        }
    }
}