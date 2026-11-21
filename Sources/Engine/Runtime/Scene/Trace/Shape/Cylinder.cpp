#include "Chicane/Runtime/Scene/Trace/Shape/Cylinder.hpp"

#include "Chicane/Runtime/Scene/Trace/Request.hpp"
#include "Chicane/Runtime/Scene/Trace/Shape/Utility.hpp"

#include <memory>

#include <algorithm>
#include <cfloat>

namespace Chicane
{
    SceneTraceShapeCylinder::SceneTraceShapeCylinder(float inRadius, std::uint32_t inSegmentCount)
        : radius(std::max(0.0f, inRadius)),
          segmentCount(std::max(3u, inSegmentCount))
    {}

    bool SceneTraceShapeCylinder::isValid() const
    {
        return radius > FLT_EPSILON;
    }

    float SceneTraceShapeCylinder::getRadiusAt(float) const
    {
        return std::max(0.0f, radius);
    }

    bool SceneTraceShapeCylinder::intersects(
        const Vec3& inOrigin, const Vec3& inDestination, const Bounds3D& inBounds, float& outEnter
    ) const
    {
        if (!isValid() || SceneTraceShapeUtility::axisLength(inOrigin, inDestination) <= FLT_EPSILON)
        {
            return false;
        }

        return SceneTraceShapeUtility::intersectsRadial(inOrigin, inDestination, inBounds, getRadiusAt(0.0f), outEnter);
    }

    SceneTraceRequest SceneTraceRequest::sCylinder(float inRadius, float inCellSize, std::uint32_t inSegmentCount)
    {
        SceneTraceRequest request;
        request.shape    = std::make_shared<SceneTraceShapeCylinder>(inRadius, inSegmentCount);
        request.cellSize = std::max(inCellSize, FLT_EPSILON);

        return request;
    }

    SceneTraceRequest SceneTraceRequest::sCylinder(
        const Vec3& inOrigin, const Vec3& inDestination, float inRadius, float inCellSize, std::uint32_t inSegmentCount
    )
    {
        SceneTraceRequest request = sCylinder(inRadius, inCellSize, inSegmentCount);
        request.origin            = inOrigin;
        request.destination       = inDestination;

        return request;
    }
}
