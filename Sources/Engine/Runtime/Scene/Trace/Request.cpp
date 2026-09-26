#include "Chicane/Runtime/Scene/Trace/Request.hpp"

#include "Chicane/Runtime/Scene/Trace/Shape/Cone.hpp"
#include "Chicane/Runtime/Scene/Trace/Shape/Cylinder.hpp"
#include "Chicane/Runtime/Scene/Trace/Shape/Line.hpp"
#include "Chicane/Runtime/Scene/Trace/Shape/Rectangle.hpp"

#include <algorithm>
#include <cfloat>
#include <cmath>

namespace Chicane
{
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

    SceneTraceRequest SceneTraceRequest::sRectangle(const Vec2& inHalfExtents, float inCellSize)
    {
        SceneTraceRequest request;
        request.shape    = std::make_shared<SceneTraceShapeRectangle>(inHalfExtents);
        request.cellSize = std::max(inCellSize, FLT_EPSILON);

        return request;
    }

    SceneTraceRequest SceneTraceRequest::sRectangle(
        const Vec3& inOrigin, const Vec3& inDestination, const Vec2& inHalfExtents, float inCellSize
    )
    {
        SceneTraceRequest request = sRectangle(inHalfExtents, inCellSize);
        request.origin            = inOrigin;
        request.destination       = inDestination;

        return request;
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

    bool SceneTraceRequest::isValid() const
    {
        return shape && shape->isValid() && cellSize > FLT_EPSILON && getLength() > FLT_EPSILON;
    }

    float SceneTraceRequest::getLength() const
    {
        const Vec3 delta = destination - origin;

        return std::sqrt(delta.dot(delta));
    }

    Vec3 SceneTraceRequest::getDirection() const
    {
        const Vec3  delta  = destination - origin;
        const float length = std::sqrt(delta.dot(delta));
        if (length <= FLT_EPSILON)
        {
            return Vec3::sZero();
        }

        return delta / length;
    }

    bool SceneTraceRequest::intersects(const Bounds3D& inBounds, float& outEnter) const
    {
        if (!isValid())
        {
            return false;
        }

        return shape->intersects(origin, destination, inBounds, outEnter);
    }
}
