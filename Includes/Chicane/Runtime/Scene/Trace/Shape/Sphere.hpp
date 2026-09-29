#pragma once

#include "Chicane/Runtime/Scene/Trace/Shape.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME SceneTraceShapeSphere : public SceneTraceShape
    {
    public:
        explicit SceneTraceShapeSphere(float inRadius);
        SceneTraceShapeSphere() = default;

    public:
        bool isValid() const override;
        bool intersects(
            const Vec3& inOrigin, const Vec3& inDestination, const Bounds3D& inBounds, float& outEnter
        ) const override;

    public:
        float radius = 0.0f;
    };
}
