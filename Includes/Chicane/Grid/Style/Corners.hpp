#pragma once

#include "Chicane/Grid/Essential.hpp"
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
                std::string top    = "0px";
                std::string bottom = "0px";
                std::string left   = "0px";
                std::string right  = "0px";
            };
        }
    }
}