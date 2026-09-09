#pragma once

#include "Chicane/Core/Math/Vec/Vec2.hpp"

#include "Chicane/Runtime/Scene/Trace/Shape.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME SceneTraceShapeRectangle : public SceneTraceShape
    {
    public:
        explicit SceneTraceShapeRectangle(const Vec2& inHalfExtents);
        SceneTraceShapeRectangle() = default;

    public:
        bool isValid() const override;
        bool intersects(
            const Vec3& inOrigin, const Vec3& inDestination, const Bounds3D& inBounds, float& outEnter
        ) const override;

    public:
        Vec2 halfExtents = Vec2::Zero();
    };
}
