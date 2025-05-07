#pragma once

#include "Core.hpp"
#include "Grid/Style/Source.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            struct CHICANE Corners
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