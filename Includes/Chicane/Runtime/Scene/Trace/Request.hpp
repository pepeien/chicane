#pragma once

#include <memory>

#include "Chicane/Core/Math/Bounds/3D.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Trace/Shape.hpp"
#include "Chicane/Runtime/Scene/Trace/Shape/Line.hpp"

namespace Chicane
{
    struct CHICANE_RUNTIME SceneTraceRequest
    {
    public:
        static constexpr inline float DefaultCellSize = 64.0f;

    public:
        static SceneTraceRequest Line(float inCellSize = DefaultCellSize);
        static SceneTraceRequest Line(
            const Vec3& inOrigin, const Vec3& inDestination, float inCellSize = DefaultCellSize
        );

        static SceneTraceRequest Rectangle(const Vec2& inHalfExtents, float inCellSize = DefaultCellSize);
        static SceneTraceRequest Rectangle(
            const Vec3& inOrigin,
            const Vec3& inDestination,
            const Vec2& inHalfExtents,
            float       inCellSize = DefaultCellSize
        );

        static SceneTraceRequest Cone(float inAngle, float inCellSize = DefaultCellSize);
        static SceneTraceRequest Cone(
            const Vec3& inOrigin, const Vec3& inDestination, float inAngle, float inCellSize = DefaultCellSize
        );

        static SceneTraceRequest Cylinder(float inRadius, float inCellSize = DefaultCellSize);
        static SceneTraceRequest Cylinder(
            const Vec3& inOrigin, const Vec3& inDestination, float inRadius, float inCellSize = DefaultCellSize
        );

    public:
        bool isValid() const;
        float getLength() const;
        Vec3 getDirection() const;

        bool intersects(const Bounds3D& inBounds, float& outEnter) const;

    public:
        std::shared_ptr<SceneTraceShape> shape       = std::make_shared<SceneTraceShapeLine>();
        float                            cellSize    = DefaultCellSize;
        Vec3                             origin      = Vec3::Zero();
        Vec3                             destination = Vec3::Zero();
    };
}
