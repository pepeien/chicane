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
            using Map = std::unordered_map<char32_t, FontGlyph>;

        public:
            static const FontGlyph& empty()
            {
                static const FontGlyph result;

                return result;
            }

        public:
            char32_t                   code     = 0U;
            float                      units    = 0U;
            float                      width    = 0U;
            float                      height   = 0U;
            float                      advance  = 0.0f;
            float                      ascender = 0.0f;
            Vec2                       bearing  = Vec2::Zero;
            std::vector<Vec3>          vertices = {};
            std::vector<std::uint32_t> indices  = {};
        };
    }
}