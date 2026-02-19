#include "Chicane/Core/Math/Curve.hpp"

namespace Chicane
{
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
            const float t  = static_cast<float>(i) / m_segmentCount;
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
            const float t  = static_cast<float>(i) / m_segmentCount;
            const float mt = 1.0f - t;

            // Cubic bezier formula: B(t) = (1-t)³P₀ + 3(1-t)²tP₁ + 3(1-t)t²P₂ +
            // t³P₃
            Vec2 point = {};
            point += mt * mt * mt * start;
            point += 3.0f * mt * mt * t * inControlA;
            point += 3.0f * mt * t * t * inControlB;
            point += t * t * t * inPoint;

            addPoint(point);
        }
    }
}