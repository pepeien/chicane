#pragma once

#include <array>
#include <cstdint>
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
            Vec2                view              = Vec2::sZero();
            Vec2                scale             = Vec2::sZero();
            Vec2                size              = Vec2::sZero();
            Vec2                offset            = Vec2::sZero();
            Vec3                position          = Vec3::sZero();
            float               backdropBlur      = 0.0f;
            Vec4                color             = Vec4::sZero();
            Vec4                clip              = Vec4::sSentinel();
            Draw::Id            texture           = -1;
            Draw::Id            glyph             = -1;
            float               dilation          = 0.0f;
            float               filterBlur        = 0.0f;
            Vec2                transformX        = Vec2::sRight();
            Vec2                transformY        = Vec2::sUp();
            Vec4                radiusX           = Vec4::sZero();
            Vec4                radiusY           = Vec4::sZero();
            Vec4                innerClip         = Vec4::sSentinel();
            Vec4                innerClipRadiusX  = Vec4::sZero();
            Vec4                innerClipRadiusY  = Vec4::sZero();
            Vec4                outerClip         = Vec4::sSentinel();
            Vec4                outerClipRadiusX  = Vec4::sZero();
            Vec4                outerClipRadiusY  = Vec4::sZero();
            Vec4                borderWidth       = Vec4::sZero();
            Vec4                borderColorTop    = Vec4::sZero();
            Vec4                borderColorRight  = Vec4::sZero();
            Vec4                borderColorBottom = Vec4::sZero();
            Vec4                borderColorLeft   = Vec4::sZero();
            std::int32_t        gradientType      = 0;
            std::int32_t        gradientStopCount = 0;
            Vec2                gradientAxis      = Vec2::sZero();
            std::array<Vec4, 8> gradientStops     = {};
            Vec4                gradientOffsets0  = Vec4::sZero();
            Vec4                gradientOffsets1  = Vec4::sZero();
        };
    }
}