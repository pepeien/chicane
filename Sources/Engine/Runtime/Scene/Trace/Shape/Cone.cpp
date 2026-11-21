#include "Chicane/Runtime/Scene/Trace/Shape/Cone.hpp"

#include "Chicane/Runtime/Scene/Trace/Request.hpp"
#include "Chicane/Runtime/Scene/Trace/Shape/Utility.hpp"

#include <memory>

#include <algorithm>
#include <cfloat>
#include <cmath>

namespace Chicane
{
    SceneTraceShapeCone::SceneTraceShapeCone(float inAngle, float inRadius, std::uint32_t inSegmentCount)
        : angle(std::max(0.0f, inAngle)),
          radius(std::max(0.0f, inRadius)),
          segmentCount(std::max(3u, inSegmentCount))
    {}

    bool SceneTraceShapeCone::isValid() const
    {
        return angle > FLT_EPSILON || radius > FLT_EPSILON;
    }

    float SceneTraceShapeCone::getRadiusAt(float inFraction, float inAxisLength) const
    {
        const float fraction = SceneTraceShapeUtility::clamp(inFraction, 0.0f, 1.0f);
        if (radius > FLT_EPSILON)
        {
            return radius * fraction;
        }

        return std::max(0.0f, inAxisLength) * std::tan(angle) * fraction;
    }

    bool SceneTraceShapeCone::intersects(
        const Vec3& inOrigin, const Vec3& inDestination, const Bounds3D& inBounds, float& outEnter
    ) const
    {
        if (!isValid())
        {
            return false;
        }

        const float length = SceneTraceShapeUtility::axisLength(inOrigin, inDestination);
        if (length <= FLT_EPSILON)
        {
            return false;
        }

        const Vec3 center   = inBounds.getCenter();
        float      fraction = 0.0f;
        SceneTraceShapeUtility::closestPointOnSegment(center, inOrigin, inDestination, fraction);

        return SceneTraceShapeUtility::intersectsRadial(
            inOrigin,
            inDestination,
            inBounds,
            getRadiusAt(fraction, length),
            outEnter
        );
    }

    SceneTraceRequest SceneTraceRequest::sCone(float inAngle, float inCellSize, std::uint32_t inSegmentCount)
    {
        SceneTraceRequest request;
        request.shape    = std::make_shared<SceneTraceShapeCone>(inAngle, 0.0f, inSegmentCount);
        request.cellSize = std::max(inCellSize, FLT_EPSILON);

        return request;
    }

    SceneTraceRequest SceneTraceRequest::sCone(
        const Vec3& inOrigin, const Vec3& inDestination, float inAngle, float inCellSize, std::uint32_t inSegmentCount
    )
    {
        SceneTraceRequest request = sCone(inAngle, inCellSize, inSegmentCount);
        request.origin            = inOrigin;
        request.destination       = inDestination;

        return request;
    }
}
