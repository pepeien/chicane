#pragma once

#include "Chicane/Core/Math/Bounds/3D.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Trace/Response.hpp"

namespace Chicane
{
    namespace SceneTraceShapeUtility
    {
        CHICANE_RUNTIME float clamp(float inValue, float inMin, float inMax);
        CHICANE_RUNTIME float lengthSquared(const Vec3& inValue);
        CHICANE_RUNTIME float axisLength(const Vec3& inOrigin, const Vec3& inDestination);

        CHICANE_RUNTIME Vec3
        closestPointOnSegment(const Vec3& inPoint, const Vec3& inStart, const Vec3& inEnd, float& outFraction);

        CHICANE_RUNTIME float distancePointToBounds(const Vec3& inPoint, const Bounds3D& inBounds);
        CHICANE_RUNTIME Vec3 closestPointOnBounds(const Vec3& inPoint, const Bounds3D& inBounds);

        CHICANE_RUNTIME bool buildAxisBasis(const Vec3& inDirection, Vec3& outRight, Vec3& outUp);

        CHICANE_RUNTIME bool intersectsRadial(
            const Vec3&     inOrigin,
            const Vec3&     inDestination,
            const Bounds3D& inBounds,
            float           inRadiusAtFraction,
            float&          outEnter
        );

        CHICANE_RUNTIME void fillResponse(
            SceneTraceResponse& outResponse,
            const Vec3&         inOrigin,
            const Vec3&         inDestination,
            const Bounds3D&     inBounds,
            float               inEnter
        );
    }
}
