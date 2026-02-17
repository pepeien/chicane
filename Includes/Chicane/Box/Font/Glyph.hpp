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
            static const FontGlyph& empty()
            {
                static const FontGlyph result;

                return result;
            }

        public:
            std::uint32_t              code     = 0U;
            std::uint32_t              units    = 0U;
            std::uint32_t              width    = 0U;
            std::uint32_t              height   = 0U;
            float                      advance  = 0.0f;
            Vec<2, std::uint32_t>      bearing  = Vec<2, std::uint32_t>(0U);
            std::vector<Vec3>          vertices = {};
            std::vector<std::uint32_t> indices  = {};
        };
    }
}