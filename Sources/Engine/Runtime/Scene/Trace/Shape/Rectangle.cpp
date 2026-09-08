#include "Chicane/Runtime/Scene/Trace/Shape/Rectangle.hpp"

#include "Chicane/Runtime/Scene/Trace/Shape/Utility.hpp"

#include <algorithm>
#include <cfloat>
#include <cmath>

namespace Chicane
{
    SceneTraceShapeRectangle::SceneTraceShapeRectangle(const Vec2& inHalfExtents)
        : halfExtents(std::max(0.0f, inHalfExtents.x), std::max(0.0f, inHalfExtents.y))
    {}

    bool SceneTraceShapeRectangle::isValid() const
    {
        return halfExtents.x > FLT_EPSILON || halfExtents.y > FLT_EPSILON;
    }

    bool SceneTraceShapeRectangle::intersects(
        const Vec3& inOrigin,
        const Vec3& inDestination,
        const Bounds3D& inBounds,
        float& outEnter
    ) const
    {
        if (!isValid())
        {
            return false;
        }

        Vec3       right;
        Vec3       up;
        const Vec3 direction = inDestination - inOrigin;
        if (!SceneTraceShapeUtility::buildAxisBasis(direction, right, up))
        {
            return false;
        }

        const Vec3 center   = inBounds.getCenter();
        float      fraction = 0.0f;
        SceneTraceShapeUtility::closestPointOnSegment(center, inOrigin, inDestination, fraction);

        const Vec3  local  = center - (inOrigin + direction * fraction);
        const float localX = local.dot(right);
        const float localY = local.dot(up);

        const Vec3& min         = inBounds.getMin().transformed;
        const Vec3& max         = inBounds.getMax().transformed;
        const float extentX     = 0.5f * (max.x - min.x);
        const float extentY     = 0.5f * (max.y - min.y);
        const float extentZ     = 0.5f * (max.z - min.z);
        const float actorRadius = std::sqrt(extentX * extentX + extentY * extentY + extentZ * extentZ);

        if (std::abs(localX) > halfExtents.x + actorRadius ||
            std::abs(localY) > halfExtents.y + actorRadius)
        {
            return false;
        }

        outEnter = fraction;

        return true;
    }
}
