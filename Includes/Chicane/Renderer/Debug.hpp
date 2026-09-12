#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "Chicane/Core/Math/Bounds/3D.hpp"
#include "Chicane/Core/Math/Quat/QuatFloat.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"
#include "Chicane/Core/Math/Vertex.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Debug/Mode.hpp"
#include "Chicane/Renderer/Debug/Trace.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace Debug
        {
            static constexpr inline std::size_t TRACE_CAPACITY = 32;

            static inline const Vec4            MESH_COLOR           = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
            static inline const Vec4            BOUNDS_COLOR         = Vec4(1.0f, 0.75f, 0.2f, 1.0f);
            static inline const Vec4            TRACE_COLOR          = Vec4(0.2f, 0.9f, 1.0f, 1.0f);
            static inline const Vec4            COLLIDER_COLOR       = Vec4(0.3f, 1.0f, 0.4f, 1.0f);
            static inline const Vec4            SKELETON_COLOR       = Vec4(0.0f, 0.82f, 1.0f, 1.0f);
            static inline const Vec4            SKELETON_JOINT_COLOR = Vec4(0.35f, 0.95f, 1.0f, 1.0f);

            CHICANE_RENDERER void appendSegment(
                Vertex::List& outVertices, const Vec3& inStart, const Vec3& inEnd, const Vec4& inColor
            );
            CHICANE_RENDERER void appendAxes(
                Vertex::List& outVertices, const Vec3& inCenter, float inSize, const Vec4& inColor
            );
            CHICANE_RENDERER void appendBone(
                Vertex::List& outVertices, const Vec3& inStart, const Vec3& inEnd, float inRadius, const Vec4& inColor
            );
            CHICANE_RENDERER void appendSphere(
                Vertex::List& outVertices, const Vec3& inCenter, float inRadius, const Vec4& inColor
            );
            CHICANE_RENDERER void appendBounds(
                Vertex::List& outVertices, const Bounds3D& inBounds, const Vec4& inColor
            );
            CHICANE_RENDERER void appendRing(
                Vertex::List&  outVertices,
                const Vec3&    inCenter,
                const Vec3&    inRight,
                const Vec3&    inUp,
                float          inRadius,
                const Vec4&    inColor,
                std::uint32_t  inSegmentCount = 16
            );
            CHICANE_RENDERER void appendRadial(
                Vertex::List&  outVertices,
                const Vec3&    inOrigin,
                const Vec3&    inDestination,
                float          inStartRadius,
                float          inEndRadius,
                const Vec4&    inColor,
                std::uint32_t  inSegmentCount = 16
            );
            CHICANE_RENDERER void appendRectangle(
                Vertex::List& outVertices,
                const Vec3&   inOrigin,
                const Vec3&   inDestination,
                const Vec2&   inHalfExtent,
                const Vec4&   inColor
            );
            CHICANE_RENDERER void appendBox(
                Vertex::List&    outVertices,
                const Vec3&      inCenter,
                const QuatFloat& inRotation,
                const Vec3&      inHalfExtents,
                const Vec4&      inColor
            );
            CHICANE_RENDERER void appendCapsule(
                Vertex::List&    outVertices,
                const Vec3&      inCenter,
                const QuatFloat& inRotation,
                float            inRadius,
                float            inHalfHeight,
                const Vec4&      inColor
            );
        }
    }
}
