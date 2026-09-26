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
        static SceneTraceRequest sLine(float inCellSize = DEFAULT_CELL_SIZE);
        static SceneTraceRequest sLine(
            const Vec3& inOrigin, const Vec3& inDestination, float inCellSize = DEFAULT_CELL_SIZE
        );

        static SceneTraceRequest sRectangle(const Vec2& inHalfExtents, float inCellSize = DEFAULT_CELL_SIZE);
        static SceneTraceRequest sRectangle(
            const Vec3& inOrigin,
            const Vec3& inDestination,
            const Vec2& inHalfExtents,
            float       inCellSize = DEFAULT_CELL_SIZE
        );

        static SceneTraceRequest sCone(
            float inAngle, float inCellSize = DEFAULT_CELL_SIZE, std::uint32_t inSegmentCount = DEFAULT_SEGEMENT_COUNT
        );
        static SceneTraceRequest sCone(
            const Vec3&   inOrigin,
            const Vec3&   inDestination,
            float         inAngle,
            float         inCellSize     = DEFAULT_CELL_SIZE,
            std::uint32_t inSegmentCount = DEFAULT_SEGEMENT_COUNT
        );

        static SceneTraceRequest sCylinder(
            float inRadius, float inCellSize = DEFAULT_CELL_SIZE, std::uint32_t inSegmentCount = DEFAULT_SEGEMENT_COUNT
        );
        static SceneTraceRequest sCylinder(
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
        Vec3                             origin      = Vec3::sZero();
        Vec3                             destination = Vec3::sZero();
    };
}
