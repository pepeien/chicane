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
    SceneTraceRequest SceneTraceRequest::Line(float inCellSize)
    {
        SceneTraceRequest request;
        request.shape    = std::make_shared<SceneTraceShapeLine>();
        request.cellSize = std::max(inCellSize, FLT_EPSILON);

        return request;
    }

    SceneTraceRequest SceneTraceRequest::Line(const Vec3& inOrigin, const Vec3& inDestination, float inCellSize)
    {
        SceneTraceRequest request = Line(inCellSize);
        request.origin            = inOrigin;
        request.destination       = inDestination;

        return request;
    }

    SceneTraceRequest SceneTraceRequest::Rectangle(const Vec2& inHalfExtents, float inCellSize)
    {
        SceneTraceRequest request;
        request.shape    = std::make_shared<SceneTraceShapeRectangle>(inHalfExtents);
        request.cellSize = std::max(inCellSize, FLT_EPSILON);

        return request;
    }

    SceneTraceRequest SceneTraceRequest::Rectangle(
        const Vec3& inOrigin, const Vec3& inDestination, const Vec2& inHalfExtents, float inCellSize
    )
    {
        SceneTraceRequest request = Rectangle(inHalfExtents, inCellSize);
        request.origin            = inOrigin;
        request.destination       = inDestination;

        return request;
    }

    SceneTraceRequest SceneTraceRequest::Cone(float inAngle, float inCellSize)
    {
        SceneTraceRequest request;
        request.shape    = std::make_shared<SceneTraceShapeCone>(inAngle);
        request.cellSize = std::max(inCellSize, FLT_EPSILON);

        return request;
    }

    SceneTraceRequest SceneTraceRequest::Cone(
        const Vec3& inOrigin, const Vec3& inDestination, float inAngle, float inCellSize
    )
    {
        SceneTraceRequest request = Cone(inAngle, inCellSize);
        request.origin            = inOrigin;
        request.destination       = inDestination;

        return request;
    }

    SceneTraceRequest SceneTraceRequest::Cylinder(float inRadius, float inCellSize)
    {
        SceneTraceRequest request;
        request.shape    = std::make_shared<SceneTraceShapeCylinder>(inRadius);
        request.cellSize = std::max(inCellSize, FLT_EPSILON);

        return request;
    }

    SceneTraceRequest SceneTraceRequest::Cylinder(
        const Vec3& inOrigin, const Vec3& inDestination, float inRadius, float inCellSize
    )
    {
        SceneTraceRequest request = Cylinder(inRadius, inCellSize);
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
            return Vec3::Zero();
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
