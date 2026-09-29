#include "Chicane/Runtime/Scene/Trace/Shape/Sphere.hpp"

#include "Chicane/Runtime/Scene/Trace/Shape/Utility.hpp"

#include <algorithm>
#include <cfloat>

namespace Chicane
{
    SceneTraceShapeSphere::SceneTraceShapeSphere(float inRadius)
        : radius(std::max(0.0f, inRadius))
    {}

    bool SceneTraceShapeSphere::isValid() const
    {
        return radius > FLT_EPSILON;
    }

    bool SceneTraceShapeSphere::intersects(
        const Vec3& inOrigin, const Vec3& inDestination, const Bounds3D& inBounds, float& outEnter
    ) const
    {
        if (!isValid() || SceneTraceShapeUtility::axisLength(inOrigin, inDestination) <= FLT_EPSILON)
        {
            return false;
        }

        if (SceneTraceShapeUtility::distancePointToBounds(inOrigin, inBounds) > radius)
        {
            return false;
        }

        SceneTraceShapeUtility::closestPointOnSegment(inBounds.getCenter(), inOrigin, inDestination, outEnter);

        return true;
    }
}
