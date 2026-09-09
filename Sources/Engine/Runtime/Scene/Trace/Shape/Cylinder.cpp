#include "Chicane/Runtime/Scene/Trace/Shape/Cylinder.hpp"

#include "Chicane/Runtime/Scene/Trace/Shape/Utility.hpp"

#include <algorithm>
#include <cfloat>

namespace Chicane
{
    SceneTraceShapeCylinder::SceneTraceShapeCylinder(float inRadius)
        : radius(std::max(0.0f, inRadius))
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
}
