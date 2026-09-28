#pragma once

#include "Chicane/Core/Math/Vec/Vec3.hpp"

#include "Chicane/Runtime/Scene/Trace/Shape.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME SceneTraceShapeBox : public SceneTraceShape
    {
    public:
        explicit SceneTraceShapeBox(const Vec3& inHalfExtents);
        SceneTraceShapeBox() = default;

    public:
        bool isValid() const override;
        bool intersects(
            const Vec3& inOrigin, const Vec3& inDestination, const Bounds3D& inBounds, float& outEnter
        ) const override;

    public:
        Vec3 halfExtents = Vec3::sZero();
    };
}
