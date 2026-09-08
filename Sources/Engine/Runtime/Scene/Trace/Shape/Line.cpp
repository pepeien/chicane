#include "Chicane/Runtime/Scene/Trace/Shape/Line.hpp"

#include "Chicane/Runtime/Scene/Trace/Shape/Utility.hpp"

namespace Chicane
{
    bool SceneTraceShapeLine::isValid() const
    {
        return true;
    }

    bool SceneTraceShapeLine::intersects(
        const Vec3& inOrigin,
        const Vec3& inDestination,
        const Bounds3D& inBounds,
        float& outEnter
    ) const
    {
        if (SceneTraceShapeUtility::axisLength(inOrigin, inDestination) <= FLT_EPSILON)
        {
            return false;
        }

        return inBounds.intersectsSegment(inOrigin, inDestination, outEnter);
    }
}
