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
                Vec2                       advance  = {};
                Vec2                       bearing  = {};
                std::vector<Vec3>          vertices = {};
                std::vector<std::uint32_t> indices  = {};
            };
        }
    }
}