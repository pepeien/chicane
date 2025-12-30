#pragma once

#include "Chicane/Box/Essential.hpp"

namespace Chicane
{
    namespace Box
    {
        struct CHICANE_BOX FontGlyph
        {
        public:
            using Map = std::unordered_map<std::uint32_t, FontGlyph>;

        public:
            std::uint32_t              code     = 0U;
            std::uint32_t              units    = 0U;
            float                      width    = 0.0f;
            float                      height   = 0.0f;
            Vec2                       advance  = {};
            Vec2                       bearing  = {};
            std::vector<Vec3>          vertices = {};
            std::vector<std::uint32_t> indices  = {};
        };
    }
}