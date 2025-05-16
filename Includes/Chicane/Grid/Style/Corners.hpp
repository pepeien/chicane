#pragma once

#include "Chicane/Grid/Style/Essential.hpp"
#include "Chicane/Grid/Style/Source.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            struct CHICANE_GRID Corners
            {
            public:
                void update(
                    const Properties &inSource,
                    const std::string &inOnelineAttributeName,
                    const std::string &inTopAttributeName,
                    const std::string &inBottomAttributeName,
                    const std::string &inLeftAttributeName,
                    const std::string &inRightAttributeName
                );

            public:
                std::string top    = CORNER_DEFAULT_VALUE;
                std::string bottom = CORNER_DEFAULT_VALUE;
                std::string left   = CORNER_DEFAULT_VALUE;
                std::string right  = CORNER_DEFAULT_VALUE;
            };
        }
    }
}