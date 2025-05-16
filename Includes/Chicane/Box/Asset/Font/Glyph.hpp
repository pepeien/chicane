#pragma once

#include "Chicane/Box/Essential.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Font
        {
            struct CHICANE_BOX Glyph
            {
            public:
                std::uint32_t              code     = 0;
                Vec<2, float>              advance  = {};
                Vec<2, float>              bearing  = {};
                std::vector<Vec<3, float>> vertices = {};
                std::vector<std::uint32_t> indices  = {};
            };
        }
    }
}