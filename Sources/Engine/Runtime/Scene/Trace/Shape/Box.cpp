#include "Chicane/Runtime/Scene/Trace/Shape/Box.hpp"

#include "Chicane/Core/Math/Mat/Mat4.hpp"
#include "Chicane/Runtime/Scene/Trace/Shape/Utility.hpp"

#include <algorithm>
#include <cfloat>
#include <cmath>

namespace Chicane
{
    SceneTraceShapeBox::SceneTraceShapeBox(const Vec3& inHalfExtents)
        : halfExtents(std::max(0.0f, inHalfExtents.x), std::max(0.0f, inHalfExtents.y), std::max(0.0f, inHalfExtents.z))
    {}

    bool SceneTraceShapeBox::isValid() const
    {
        return halfExtents.x > FLT_EPSILON || halfExtents.y > FLT_EPSILON || halfExtents.z > FLT_EPSILON;
    }

    bool SceneTraceShapeBox::intersects(
        const Vec3& inOrigin, const Vec3& inDestination, const Bounds3D& inBounds, float& outEnter
    ) const
    {
        if (!isValid() || SceneTraceShapeUtility::axisLength(inOrigin, inDestination) <= FLT_EPSILON)
        {
            return false;
        }

        Bounds3D box = Bounds3D::sBox(halfExtents * 2.0f);
        box.transform(Mat4::sTranslate(inOrigin));
        if (!box.intersects(inBounds))
        {
            return false;
        }

        SceneTraceShapeUtility::closestPointOnSegment(inBounds.getCenter(), inOrigin, inDestination, outEnter);

        return true;
    }
}
