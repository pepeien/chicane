#include "Chicane/Core/Math/Curve.hpp"

#include <earcut.hpp>

namespace Chicane
{
    std::vector<std::vector<std::array<float, 2>>> getPolygons(const std::vector<Curve>& inContours)
    {
        std::vector<std::vector<std::array<float, 2>>> result = {};

        for (const Curve& curve : inContours)
        {
            if (curve.isEmpty())
            {
                continue;
            }

            std::vector<std::array<float, 2>> contour = {};

            for (const Vec<2, float>& point : curve.getPoints())
            {
                contour.push_back({ point.x, point.y });
            }

            result.push_back(contour);
        }

        return result;
    }

    std::vector<Vec<3, float>> Curve::getTriangleVertices(
        const std::vector<Curve>& inContours,
        float inPixelSize
    )
    {
        if (inContours.empty())
        {
            return {};
        }

        const float fixedPoint = inPixelSize * 64.0f;

        std::vector<Vec<3, float>> result = {};

        for (const Curve& curve : inContours)
        {
            if (curve.isEmpty())
            {
                continue;
            }

            for (const Vec<2, float>& point : curve.getPoints())
            {
                result.push_back({ point.x / fixedPoint, point.y / fixedPoint, 0.0f });
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

        std::vector<std::vector<std::array<float, 2>>> polygons = {};

        const auto outerPolygons = getPolygons(outerContours);
        polygons.insert(
            polygons.end(),
            outerPolygons.begin(),
            outerPolygons.end()
        );

        const auto holePolygons = getPolygons(holeContours);
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

    void Curve::addQuadraticPoint(
        const Vec<2, float>& inControl,
        const Vec<2, float>& inPoint
    )
    {
        if (isEmpty() || m_segmentCount <= 0)
        {
            return;
        }

        const Vec<2, float>& startPoint = getEndPoint();

        for (int i = 1; i <= m_segmentCount; i++)
        {
            float t = static_cast<float>(i) / m_segmentCount;
            float mt = 1.0f - t;

            // Quadratic bezier formula: B(t) = (1-t)²P₀ + 2(1-t)tP₁ + t²P₂
            Vec<2, float> point = {};
            point += mt * mt * startPoint;
            point += 2.0f * mt * t * inControl;
            point += t * t * inPoint;

            addPoint(point);
        }
    }

    void Curve::addBezierPoint(
        const Vec<2, float>& inControlA,
        const Vec<2, float>& inControlB,
        const Vec<2, float>& inPoint
    )
    {
        if (isEmpty() || m_segmentCount <= 0)
        {
            return;
        }

        const Vec<2, float>& startPoint = getEndPoint();

        for (int i = 1; i <= m_segmentCount; i++)
        {
            float t = static_cast<float>(i) / m_segmentCount;
            float mt = 1.0f - t;

            // Cubic bezier formula: B(t) = (1-t)³P₀ + 3(1-t)²tP₁ + 3(1-t)t²P₂ + t³P₃
            Vec<2, float> point = {};
            point += mt * mt * mt * startPoint;
            point += 3.0f * mt * mt * t * inControlA;
            point += 3.0f * mt * t * t * inControlB;
            point += t * t * t * inPoint;

            addPoint(point);
        }
    }

    bool Curve::isHole() const
    {
        if (m_points.size() < 3) {
            return true;
        }

        std::size_t pointCount = m_points.size();

        double area = 0.0;

        for (std::size_t i = 0; i < pointCount; ++i) {
            const Vec<2, float>& point        = m_points.at(i);
            const Vec<2, float>& nearestPoint = m_points.at((i + 1) % pointCount);

            area += static_cast<double>(
                (point.x - nearestPoint.y) * (nearestPoint.x - point.y)
            );
        }

        return (area / 2.0) < 0;
    }
}