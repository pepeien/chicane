#include "Chicane/Runtime/Scene/Trace/Shape/Line.hpp"

#include "Chicane/Runtime/Scene/Trace/Request.hpp"
#include "Chicane/Runtime/Scene/Trace/Shape/Utility.hpp"

#include <algorithm>
#include <cfloat>
#include <memory>

namespace Chicane
{
    bool SceneTraceShapeLine::isValid() const
    {
        return true;
    }

    bool SceneTraceShapeLine::intersects(
        const Vec3& inOrigin, const Vec3& inDestination, const Bounds3D& inBounds, float& outEnter
    ) const
    {
        if (SceneTraceShapeUtility::axisLength(inOrigin, inDestination) <= FLT_EPSILON)
        {
            return false;
        }

        return inBounds.intersectsSegment(inOrigin, inDestination, outEnter);
    }

    SceneTraceRequest SceneTraceRequest::sLine(float inCellSize)
    {
        SceneTraceRequest request;
        request.shape    = std::make_shared<SceneTraceShapeLine>();
        request.cellSize = std::max(inCellSize, FLT_EPSILON);

        return request;
    }

    SceneTraceRequest SceneTraceRequest::sLine(const Vec3& inOrigin, const Vec3& inDestination, float inCellSize)
    {
        SceneTraceRequest request = sLine(inCellSize);
        request.origin            = inOrigin;
        request.destination       = inDestination;

        return request;
    }
}
