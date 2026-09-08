#pragma once

#include "Chicane/Core/Math/Bounds/3D.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME SceneTraceShape
    {
    public:
        virtual ~SceneTraceShape() = default;

    public:
        virtual bool isValid() const = 0;

        virtual bool intersects(
            const Vec3& inOrigin, const Vec3& inDestination, const Bounds3D& inBounds, float& outEnter
        ) const = 0;
    };
}
