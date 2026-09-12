#pragma once

#include <cstdint>
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
        // Values
        static constexpr inline float         DEFAULT_CELL_SIZE      = 64.0f;
        static constexpr inline float         DEFAULT_DURATION       = 2.0f;
        static constexpr inline std::uint32_t DEFAULT_SEGEMENT_COUNT = 16;

    public:
        static SceneTraceRequest Line(float inCellSize = DEFAULT_CELL_SIZE);
        static SceneTraceRequest Line(
            const Vec3& inOrigin, const Vec3& inDestination, float inCellSize = DEFAULT_CELL_SIZE
        );

        static SceneTraceRequest Rectangle(const Vec2& inHalfExtents, float inCellSize = DEFAULT_CELL_SIZE);
        static SceneTraceRequest Rectangle(
            const Vec3& inOrigin,
            const Vec3& inDestination,
            const Vec2& inHalfExtents,
            float       inCellSize = DEFAULT_CELL_SIZE
        );

        static SceneTraceRequest Cone(
            float inAngle, float inCellSize = DEFAULT_CELL_SIZE, std::uint32_t inSegmentCount = DEFAULT_SEGEMENT_COUNT
        );
        static SceneTraceRequest Cone(
            const Vec3&   inOrigin,
            const Vec3&   inDestination,
            float         inAngle,
            float         inCellSize     = DEFAULT_CELL_SIZE,
            std::uint32_t inSegmentCount = DEFAULT_SEGEMENT_COUNT
        );

        static SceneTraceRequest Cylinder(
            float inRadius, float inCellSize = DEFAULT_CELL_SIZE, std::uint32_t inSegmentCount = DEFAULT_SEGEMENT_COUNT
        );
        static SceneTraceRequest Cylinder(
            const Vec3&   inOrigin,
            const Vec3&   inDestination,
            float         inRadius,
            float         inCellSize     = DEFAULT_CELL_SIZE,
            std::uint32_t inSegmentCount = DEFAULT_SEGEMENT_COUNT
        );

    public:
        bool isValid() const;
        float getLength() const;
        Vec3 getDirection() const;

        bool intersects(const Bounds3D& inBounds, float& outEnter) const;

    public:
        std::shared_ptr<SceneTraceShape> shape       = std::make_shared<SceneTraceShapeLine>();
        float                            cellSize    = DEFAULT_CELL_SIZE;
        float                            duration    = DEFAULT_DURATION;
        Vec3                             origin      = Vec3::Zero();
        Vec3                             destination = Vec3::Zero();
    };
}
