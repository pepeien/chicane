#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Math/Line.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"

namespace Chicane
{
    struct CHICANE_CORE Curve : public Line
    {
    public:
        using Polygon = std::vector<std::array<float, 2>>;

    public:
        static std::vector<Polygon> getPolygons(const std::vector<Curve>& inContours);
        static std::vector<Vec3> getTriangleVertices(const std::vector<Curve>& inContours);
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