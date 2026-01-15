#pragma once

#include "Chicane/Box.hpp"

#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"

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
            Vec2                       scale    = {};
            Vec2                       box      = {};
            Vec2                       line     = {};
            std::vector<Vec3>          vertices = {};
            std::vector<std::uint32_t> indices  = {};
        };
    }
}