#pragma once

#include <cstdint>
#include <memory>

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Math/Bounds/3D.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Trace/Shape.hpp"
#include "Chicane/Runtime/Scene/Trace/Shape/Line.hpp"

namespace Chicane
{
    namespace Box
    {
        class Mesh;
    }

    CH_TYPE(Type = (Manual), Alias = (TraceRequest))
    struct CHICANE_RUNTIME SceneTraceRequest
    {
    public:
        // Values
        static constexpr inline const float         DEFAULT_CELL_SIZE      = 64.0f;
        static constexpr inline const float         DEFAULT_DURATION       = 2.0f;
        static constexpr inline const std::uint32_t DEFAULT_SEGEMENT_COUNT = 16;

    public:
        CH_FUNCTION()
        static inline SceneTraceRequest Line(const Vec3& inOrigin, const Vec3& inDestination)
        {
            return sLine(inOrigin, inDestination);
        }

        CH_FUNCTION()
        static inline SceneTraceRequest Rectangle(
            const Vec3& inOrigin, const Vec3& inDestination, const Vec2& inHalfExtents
        )
        {
            return sRectangle(inOrigin, inDestination, inHalfExtents);
        }

        CH_FUNCTION()
        static inline SceneTraceRequest Cone(const Vec3& inOrigin, const Vec3& inDestination, float inAngle)
        {
            return sCone(inOrigin, inDestination, inAngle);
        }

        CH_FUNCTION()
        static SceneTraceRequest Cylinder(const Vec3& inOrigin, const Vec3& inDestination, float inRadius)
        {
            return sCylinder(inOrigin, inDestination, inRadius);
        }

        CH_FUNCTION()
        static inline SceneTraceRequest Pyramid(
            const Vec3& inOrigin, const Vec3& inDestination, const Vec2& inHalfExtents
        )
        {
            return sPyramid(inOrigin, inDestination, inHalfExtents);
        }

        CH_FUNCTION()
        static inline SceneTraceRequest Box(const Vec3& inOrigin, const Vec3& inDestination, const Vec3& inHalfExtents)
        {
            return sBox(inOrigin, inDestination, inHalfExtents);
        }

        CH_FUNCTION()
        static inline SceneTraceRequest Sphere(const Vec3& inOrigin, const Vec3& inDestination, float inRadius)
        {
            return sSphere(inOrigin, inDestination, inRadius);
        }

        CH_FUNCTION()
        static inline SceneTraceRequest Mesh(
            const Vec3& inOrigin, const Vec3& inDestination, const FileSystem::Path& inMesh
        )
        {
            return sMesh(inOrigin, inDestination, inMesh);
        }

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

        static SceneTraceRequest sPyramid(const Vec2& inHalfExtents, float inCellSize = DEFAULT_CELL_SIZE);
        static SceneTraceRequest sPyramid(
            const Vec3& inOrigin,
            const Vec3& inDestination,
            const Vec2& inHalfExtents,
            float       inCellSize = DEFAULT_CELL_SIZE
        );

        static SceneTraceRequest sBox(const Vec3& inHalfExtents, float inCellSize = DEFAULT_CELL_SIZE);
        static SceneTraceRequest sBox(
            const Vec3& inOrigin,
            const Vec3& inDestination,
            const Vec3& inHalfExtents,
            float       inCellSize = DEFAULT_CELL_SIZE
        );

        static SceneTraceRequest sSphere(float inRadius, float inCellSize = DEFAULT_CELL_SIZE);
        static SceneTraceRequest sSphere(
            const Vec3& inOrigin, const Vec3& inDestination, float inRadius, float inCellSize = DEFAULT_CELL_SIZE
        );

        static SceneTraceRequest sMesh(const Box::Mesh* inMesh, float inCellSize = DEFAULT_CELL_SIZE);
        static SceneTraceRequest sMesh(
            const Vec3&      inOrigin,
            const Vec3&      inDestination,
            const Box::Mesh* inMesh,
            float            inCellSize = DEFAULT_CELL_SIZE
        );
        static SceneTraceRequest sMesh(const FileSystem::Path& inMesh, float inCellSize = DEFAULT_CELL_SIZE);
        static SceneTraceRequest sMesh(
            const Vec3&             inOrigin,
            const Vec3&             inDestination,
            const FileSystem::Path& inMesh,
            float                   inCellSize = DEFAULT_CELL_SIZE
        );

    public:
        CH_FUNCTION()
        bool isValid() const;

        CH_FUNCTION()
        float getLength() const;

        CH_FUNCTION()
        Vec3 getDirection() const;

        CH_FUNCTION()
        const Vec3& getOrigin() const { return origin; }

        CH_FUNCTION()
        void setOrigin(const Vec3& inValue) { origin = inValue; }

        CH_FUNCTION()
        const Vec3& getDestination() const { return destination; }

        CH_FUNCTION()
        void setDestination(const Vec3& inValue) { destination = inValue; }

    public:
        bool intersects(const Bounds3D& inBounds, float& outEnter) const;

    public:
        CH_FIELD()
        float cellSize = DEFAULT_CELL_SIZE;

        CH_FIELD()
        float duration = DEFAULT_DURATION;

        CH_FIELD()
        Vec3 origin = Vec3::sZero();

        CH_FIELD()
        Vec3 destination = Vec3::sZero();

    public:
        std::shared_ptr<SceneTraceShape> shape = std::make_shared<SceneTraceShapeLine>();
    };
}
