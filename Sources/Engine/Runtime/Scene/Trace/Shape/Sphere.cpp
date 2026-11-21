#include "Chicane/Runtime/Scene/Trace/Shape/Sphere.hpp"

#include "Chicane/Runtime/Scene/Trace/Request.hpp"
#include "Chicane/Runtime/Scene/Trace/Shape/Utility.hpp"

#include <algorithm>
#include <cfloat>
#include <memory>

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

    SceneTraceRequest SceneTraceRequest::sSphere(float inRadius, float inCellSize)
    {
        SceneTraceRequest request;
        request.shape    = std::make_shared<SceneTraceShapeSphere>(inRadius);
        request.cellSize = std::max(inCellSize, FLT_EPSILON);

        return request;
    }

    SceneTraceRequest SceneTraceRequest::sSphere(
        const Vec3& inOrigin, const Vec3& inDestination, float inRadius, float inCellSize
    )
    {
        SceneTraceRequest request = sSphere(inRadius, inCellSize);
        request.origin            = inOrigin;
        request.destination       = inDestination;

        return request;
    }
}
