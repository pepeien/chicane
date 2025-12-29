#include "Chicane/Core/Math/Curve.hpp"

#include <mapbox/earcut.hpp>

namespace Chicane
{
    std::vector<Curve::Polygon> Curve::getPolygons(const std::vector<Curve>& inContours)
    {
        std::vector<Polygon> result = {};

        for (const Curve& curve : inContours)
        {
            if (curve.isEmpty())
            {
                continue;
            }

            Polygon polygon = {};

            for (const Vec2& point : curve.getPoints())
            {
                polygon.push_back({ point.x, point.y });
            }

            result.push_back(polygon);
        }

        return result;
    }

    std::vector<Vec3> Curve::getTriangleVertices(const std::vector<Curve>& inContours)
    {
        if (inContours.empty())
        {
            return {};
        }

        std::vector<Vec3> result = {};

        for (const Curve& curve : inContours)
        {
            if (curve.isEmpty())
            {
                continue;
            }

            for (const Vec2& point : curve.getPoints())
            {
                result.push_back({ point.x / FIXED_POINT, point.y / FIXED_POINT, 0.0f });
            }
        }

        return result;
    }

    std::vector<std::uint32_t> Curve::getTriangleIndices(const std::vector<Curve>& inContours)
    {
        if (inContours.empty())
        {
            return {};
        }

        std::vector<Curve> outerContours = {};
        std::vector<Curve> holeContours  = {};

        for (const Curve& curve : inContours)
        {
            if (curve.isEmpty())
            {
                continue;
            }

            if (curve.isHole())
            {
                holeContours.push_back(curve);

                continue;
            }

            outerContours.push_back(curve);
        }

        if (outerContours.empty())
        {
            return {};
        }

        std::vector<Polygon> polygons = {};

        const std::vector<Polygon> outerPolygons = getPolygons(outerContours);
        polygons.insert(
            polygons.end(),
            outerPolygons.begin(),
            outerPolygons.end()
        );

        const std::vector<Polygon> holePolygons = getPolygons(holeContours);
        polygons.insert(
            polygons.end(),
            holePolygons.begin(),
            holePolygons.end()
        );

        return mapbox::earcut<std::uint32_t>(polygons);
    }

    void Curve::setSegmentCount(std::uint32_t inSegmentCount)
    {
        m_segmentCount = inSegmentCount;
    }

    void Curve::addQuadraticPoint(const Vec2& inControl, const Vec2& inPoint)
    {
        if (isEmpty() || m_segmentCount <= 0)
        {
            return;
        }

        const Vec2& start = getEndPoint();

        for (int i = 1; i <= m_segmentCount; i++)
        {
            const float t = static_cast<float>(i) / m_segmentCount;
            const float mt = 1.0f - t;

            // Quadratic bezier formula: B(t) = (1-t)²P₀ + 2(1-t)tP₁ + t²P₂
            Vec2 point = {};
            point += mt * mt * start;
            point += 2.0f * mt * t * inControl;
            point += t * t * inPoint;

            addPoint(point);
        }
    }

    void Curve::addBezierPoint(const Vec2& inControlA, const Vec2& inControlB, const Vec2& inPoint)
    {
        if (isEmpty() || m_segmentCount <= 0)
        {
            return;
        }

        const Vec2& start = getEndPoint();

        for (int i = 1; i <= m_segmentCount; i++)
        {
            const float t = static_cast<float>(i) / m_segmentCount;
            const float mt = 1.0f - t;

            // Cubic bezier formula: B(t) = (1-t)³P₀ + 3(1-t)²tP₁ + 3(1-t)t²P₂ + t³P₃
            Vec2 point = {};
            point += mt * mt * mt * start;
            point += 3.0f * mt * mt * t * inControlA;
            point += 3.0f * mt * t * t * inControlB;
            point += t * t * t * inPoint;

            addPoint(point);
        }
    }

    bool Curve::isHole() const
    {
        if (m_points.size() < 3)
        {
            return false;
        }

        double area = 0.0;

        const std::size_t count = m_points.size();

        for (std::size_t i = 0; i < count; ++i)
        {
            const Vec2& current = m_points.at(i);
            const Vec2& nearest = m_points.at((i + 1) % count);

            area += (static_cast<double>(current.x) * static_cast<double>(nearest.y)) - (static_cast<double>(nearest.x) * static_cast<double>(current.y));
        }

        return area > -(1e-6);
    }
}