#include "Chicane/Core/Math/Curve.hpp"

namespace Chicane
{
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
}