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
            Vec2     position = Vec2::Zero();
            Mat3     matrix   = Mat3(1.0f);
            Bounds2D bounds   = {};
            Bounds2D clip     = Bounds2D::unconstrained();

            float    opacity    = 1.0f;
            float    filterBlur = 0.0f;
            float    depth      = 0.0f;

            Vec4     radiusX = Vec4::Zero();
            Vec4     radiusY = Vec4::Zero();

            Vec4     innerClip        = Vec4::Sentinel();
            Vec4     innerClipRadiusX = Vec4::Zero();
            Vec4     innerClipRadiusY = Vec4::Zero();
            Vec4     outerClip        = Vec4::Sentinel();
            Vec4     outerClipRadiusX = Vec4::Zero();
            Vec4     outerClipRadiusY = Vec4::Zero();
        };
    }
}
