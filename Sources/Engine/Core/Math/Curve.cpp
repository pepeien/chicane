#include "Chicane/Core/Math/Curve.hpp"

namespace Chicane
{
    static float distanceSquared(const Vec2& inLeft, const Vec2& inRight)
    {
        const float dx = inLeft.x - inRight.x;
        const float dy = inLeft.y - inRight.y;

        return (dx * dx) + (dy * dy);
    }

    static Vec2 midpoint(const Vec2& inStart, const Vec2& inEnd)
    {
        return Vec2((inStart.x + inEnd.x) * 0.5f, (inStart.y + inEnd.y) * 0.5f);
    }

    static float pointToSegmentDistanceSquared(const Vec2& inPoint, const Vec2& inStart, const Vec2& inEnd)
    {
        const float abx = inEnd.x - inStart.x;
        const float aby = inEnd.y - inStart.y;
        const float apx = inPoint.x - inStart.x;
        const float apy = inPoint.y - inStart.y;
        const float ab2 = (abx * abx) + (aby * aby);
        const float min = Curve::FLATNESS * Curve::FLATNESS;

        if (ab2 <= min)
        {
            return distanceSquared(inPoint, inStart);
        }

        float t = (apx * abx + apy * aby) / ab2;

        if (t < 0.0f)
        {
            t = 0.0f;
        }

        if (t > 1.0f)
        {
            t = 1.0f;
        }

        return distanceSquared(inPoint, Vec2(inStart.x + (abx * t), inStart.y + (aby * t)));
    }

    static void flattenQuadratic(
        Curve&              outCurve,
        const Vec2&         inStart,
        const Vec2&         inControl,
        const Vec2&         inEnd,
        const std::uint32_t inDepth
    )
    {
        if (inDepth >= Curve::MAX_DEPTH)
        {
            outCurve.addPoint(inEnd);

            return;
        }

        const float min = Curve::FLATNESS * Curve::FLATNESS;

        if (pointToSegmentDistanceSquared(inControl, inStart, inEnd) <= min)
        {
            outCurve.addPoint(inEnd);

            return;
        }

        const Vec2 controlA = midpoint(inStart, inControl);
        const Vec2 controlB = midpoint(inControl, inEnd);
        const Vec2 split    = midpoint(controlA, controlB);

        flattenQuadratic(outCurve, inStart, controlA, split, inDepth + 1);
        flattenQuadratic(outCurve, split, controlB, inEnd, inDepth + 1);
    }

    static void flattenCubic(
        Curve&              outCurve,
        const Vec2&         inStart,
        const Vec2&         inControlA,
        const Vec2&         inControlB,
        const Vec2&         inEnd,
        const std::uint32_t inDepth
    )
    {
        if (inDepth >= Curve::MAX_DEPTH)
        {
            outCurve.addPoint(inEnd);

            return;
        }

        const float min = Curve::FLATNESS * Curve::FLATNESS;

        if (pointToSegmentDistanceSquared(inControlA, inStart, inEnd) <= min &&
            pointToSegmentDistanceSquared(inControlB, inStart, inEnd) <= min)
        {
            outCurve.addPoint(inEnd);

            return;
        }

        const Vec2 ab    = midpoint(inStart, inControlA);
        const Vec2 bc    = midpoint(inControlA, inControlB);
        const Vec2 cd    = midpoint(inControlB, inEnd);
        const Vec2 abc   = midpoint(ab, bc);
        const Vec2 bcd   = midpoint(bc, cd);
        const Vec2 split = midpoint(abc, bcd);

        flattenCubic(outCurve, inStart, ab, abc, split, inDepth + 1);
        flattenCubic(outCurve, split, bcd, cd, inEnd, inDepth + 1);
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

        flattenQuadratic(*this, getEndPoint(), inControl, inPoint, 0);
    }

    void Curve::addBezierPoint(const Vec2& inControlA, const Vec2& inControlB, const Vec2& inPoint)
    {
        if (isEmpty() || m_segmentCount <= 0)
        {
            return;
        }

        flattenCubic(*this, getEndPoint(), inControlA, inControlB, inPoint, 0);
    }
}
