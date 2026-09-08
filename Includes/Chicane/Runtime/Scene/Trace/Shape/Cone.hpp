#pragma once

#include "Chicane/Runtime/Scene/Trace/Shape.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME SceneTraceShapeCone : public SceneTraceShape
    {
    public:
        explicit SceneTraceShapeCone(float inAngle, float inRadius = 0.0f);
        SceneTraceShapeCone() = default;

    public:
        bool isValid() const override;
        bool intersects(
            const Vec3& inOrigin,
            const Vec3& inDestination,
            const Bounds3D& inBounds,
            float& outEnter
        ) const override;

    public:
        float getRadiusAt(float inFraction, float inAxisLength) const;

    public:
        float angle  = 0.0f; // Used when `radius` is zero.
        float radius = 0.0f;
    };
}
