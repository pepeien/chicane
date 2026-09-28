#pragma once

#include "Chicane/Core/Math/Vec/Vec2.hpp"

#include "Chicane/Runtime/Scene/Trace/Shape.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME SceneTraceShapePyramid : public SceneTraceShape
    {
    public:
        explicit SceneTraceShapePyramid(const Vec2& inHalfExtents);
        SceneTraceShapePyramid() = default;

    public:
        bool isValid() const override;
        bool intersects(
            const Vec3& inOrigin, const Vec3& inDestination, const Bounds3D& inBounds, float& outEnter
        ) const override;

    public:
        Vec2 getHalfExtentsAt(float inFraction) const;

    public:
        Vec2 halfExtents = Vec2::sZero();
    };
}
