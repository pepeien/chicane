#pragma once

#include "Chicane/Box.hpp"

#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Math/Vertex.hpp"

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
            char32_t        code      = 0U;
            float           width     = 0.0f;
            float           height    = 0.0f;
            float           advance   = 0.0f;
            float           ascender  = 0.0f;
            float           descender = 0.0f;
            Vec2            bearing   = Vec2::Zero;
            Vertex::List    vertices  = {};
            Vertex::Indices indices   = {};
        };
    }
}