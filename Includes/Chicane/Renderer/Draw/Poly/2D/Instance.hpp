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
            Vec2     view             = Vec2::Zero();
            Vec2     scale            = Vec2::Zero();
            Vec2     size             = Vec2::Zero();
            Vec2     offset           = Vec2::Zero();
            Vec3     position         = Vec3::Zero();
            float    _padding1        = 0.0f; // Shader alignment
            Vec4     color            = Vec4::Zero();
            Vec4     clip             = Vec4::Sentinel();
            Draw::Id texture          = -1;
            Draw::Id glyph            = -1;
            float    dilation         = 0.0f;
            float    _padding2        = 0.0f; // Shader alignment
            Vec4     radiusX          = Vec4::Zero();
            Vec4     radiusY          = Vec4::Zero();
            Vec4     innerClip        = Vec4::Sentinel();
            Vec4     innerClipRadiusX = Vec4::Zero();
            Vec4     innerClipRadiusY = Vec4::Zero();
            Vec4     outerClip        = Vec4::Sentinel();
            Vec4     outerClipRadiusX = Vec4::Zero();
            Vec4     outerClipRadiusY = Vec4::Zero();
        };
    }
}