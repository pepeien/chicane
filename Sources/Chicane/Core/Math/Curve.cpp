#include "Chicane/Core/Math/Curve.hpp"

#include <earcut.hpp>

namespace Chicane
{
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

        std::vector<Curve> sortedContours = inContours;

        std::sort(
            sortedContours.begin(),
            sortedContours.end(),
            [](const Curve& inA, const Curve& inB)
            {
                return std::abs(inA.getSignedArea()) > std::abs(inB.getSignedArea());
            }
        );

        std::vector<std::vector<std::array<float, 2>>> polygons = {};

        for (const Curve& curve : sortedContours)
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

            polygons.push_back(contour);
        }

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

        for (int i = 0; i <= m_segmentCount; i++)
        {
            float t = static_cast<float>(i) / m_segmentCount;
            float mt = 1.0f - t;

            // Quadratic bezier formula: B(t) = (1-t)²P₀ + 2(1-t)tP₁ + t²P₂
            addPoint(
                mt * mt * getEndPoint() + 
                2.0f * mt * t * inControl + 
                t * t * inPoint
            );
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

        for (int i = 0; i <= m_segmentCount; i++)
        {
            float t = static_cast<float>(i) / m_segmentCount;
            float mt = 1.0f - t;

            // Cubic bezier formula: B(t) = (1-t)³P₀ + 3(1-t)²tP₁ + 3(1-t)t²P₂ + t³P₃
            addPoint(
                mt * mt * mt * getEndPoint() + 
                3.0f * mt * mt * t * inControlA + 
                3.0f * mt * t * t * inControlB + 
                t * t * t * inPoint
            );
        }
    }

    float Curve::getSignedArea() const
    {
        double area = 0.0;

        for (size_t i = 0; i < m_points.size(); ++i) {
            const auto& p0 = m_points.at(i);
            const auto& p1 = m_points.at((i + 1) % m_points.size());

            area += (p0[0] * p1[1]) - (p1[0] * p0[1]);
        }

        return 0.5 * area;
    }
}