#pragma once

#include <map>
#include <vector>

#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawPoly2DInstance
        {
        public:
            using List = std::vector<DrawPoly2DInstance>;
            using Map  = std::map<Draw::Id, List>;

        public:
            Vec2     view     = Vec2::Zero();
            Vec2     scale    = Vec2::Zero();
            Vec2     size     = Vec2::Zero();
            Vec2     offset   = Vec2::Zero();
            Vec3     position = Vec3::Zero();
            float    _padding1; // Shader alignment
            Vec4     color   = Vec4::Zero();
            Draw::Id texture = -1;
            // Offset into the glyph outline buffer, or -1 when this instance is not a glyph
            Draw::Id glyph     = -1;
            // Quad padding around the glyph outline, in em units, so antialiasing is not clipped by the quad
            float    dilation  = 0.0f;
            float    _padding2; // Shader alignment
        };
    }
}