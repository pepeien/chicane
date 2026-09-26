#pragma once

#include "Chicane/Core/Math/Bounds/2D.hpp"
#include "Chicane/Core/Math/Mat/Mat3.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID DrawCache
        {
        public:
            Vec2     position = Vec2::sZero();
            Mat3     matrix   = Mat3(1.0f);
            Bounds2D bounds   = {};
            Bounds2D clip     = Bounds2D::sUnconstrained();

            float    opacity    = 1.0f;
            float    filterBlur = 0.0f;
            float    depth      = 0.0f;

            Vec4     radiusX = Vec4::sZero();
            Vec4     radiusY = Vec4::sZero();

            Vec4     innerClip        = Vec4::sSentinel();
            Vec4     innerClipRadiusX = Vec4::sZero();
            Vec4     innerClipRadiusY = Vec4::sZero();
            Vec4     outerClip        = Vec4::sSentinel();
            Vec4     outerClipRadiusX = Vec4::sZero();
            Vec4     outerClipRadiusY = Vec4::sZero();
        };
    }
}
