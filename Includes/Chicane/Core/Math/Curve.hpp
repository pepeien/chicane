#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Math/Line.hpp"

namespace Chicane
{
    struct CHICANE_CORE Curve : public Line
    {
    public:
        static std::vector<Vec3> getTriangleVertices(
            const std::vector<Curve>& inContours,
            float inPixelSize = 1.0f
        );
        static std::vector<std::uint32_t> getTriangleIndices(const std::vector<Curve>& inContours);

    public:
        void setSegmentCount(std::uint32_t inSegmentCount);

        void addQuadraticPoint(const Vec2& inControl, const Vec2& inPoint);
        void addBezierPoint(const Vec2& inControlA, const Vec2& inControlB, const Vec2& inPoint);

        bool isHole() const;

    protected:
        std::uint32_t m_segmentCount = 0;
    };
}