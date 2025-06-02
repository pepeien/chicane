#pragma once

#include "Chicane/Grid/Style/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            struct CHICANE_GRID Font
            {
            public:
                Font();

            public:
                std::string family;
                float       size;
            };
        }
    }
}