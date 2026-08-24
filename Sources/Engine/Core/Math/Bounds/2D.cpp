#include "Chicane/Core/Math/Bounds/2D.hpp"

#include <algorithm>

namespace Chicane
{
    Bounds2D::Bounds2D()
        : top(0.0f),
          bottom(0.0f),
          left(0.0f),
          right(0.0f),
          m_baseTop(0.0f),
          m_baseBottom(0.0f),
          m_baseLeft(0.0f),
          m_baseRight(0.0f)
    {}

    bool Bounds2D::contains(const Vec2& inPoint) const
    {
        return (inPoint.x >= left && inPoint.x <= right) && (inPoint.y >= top && inPoint.y <= bottom);
    }

    bool Bounds2D::containsRounded(
        const Vec2& inPoint, const Vec4& inHorizontalRadius, const Vec4& inVerticalRadius
    ) const
    {
        if (!contains(inPoint))
        {
            return false;
        }

        const float width  = right - left;
        const float height = bottom - top;
        const float x      = inPoint.x - left;
        const float y      = inPoint.y - top;

        const auto insideCorner = [](float inOffsetX, float inOffsetY, float inRadiusX, float inRadiusY)
        {
            if (inRadiusX <= 0.0f || inRadiusY <= 0.0f)
            {
                return true;
            }

            const float nx = inOffsetX / inRadiusX;
            const float ny = inOffsetY / inRadiusY;

            return (nx * nx) + (ny * ny) <= 1.0f;
        };

        if (x < inHorizontalRadius.x && y < inVerticalRadius.x)
        {
            return insideCorner(
                x - inHorizontalRadius.x,
                y - inVerticalRadius.x,
                inHorizontalRadius.x,
                inVerticalRadius.x
            );
        }

        if (x > width - inHorizontalRadius.y && y < inVerticalRadius.y)
        {
            return insideCorner(
                x - (width - inHorizontalRadius.y),
                y - inVerticalRadius.y,
                inHorizontalRadius.y,
                inVerticalRadius.y
            );
        }

        if (x > width - inHorizontalRadius.z && y > height - inVerticalRadius.z)
        {
            return insideCorner(
                x - (width - inHorizontalRadius.z),
                y - (height - inVerticalRadius.z),
                inHorizontalRadius.z,
                inVerticalRadius.z
            );
        }

        if (x < inHorizontalRadius.w && y > height - inVerticalRadius.w)
        {
            return insideCorner(
                x - inHorizontalRadius.w,
                y - (height - inVerticalRadius.w),
                inHorizontalRadius.w,
                inVerticalRadius.w
            );
        }

        return true;
    }

    bool Bounds2D::overlaps(const Bounds2D& inOther) const
    {
        return !(right <= inOther.left || left >= inOther.right || bottom <= inOther.top || top >= inOther.bottom);
    }

    bool Bounds2D::isEmpty() const
    {
        return right <= left || bottom <= top;
    }

    Bounds2D Bounds2D::intersect(const Bounds2D& inOther) const
    {
        Bounds2D result;
        result.top    = std::max(top, inOther.top);
        result.bottom = std::min(bottom, inOther.bottom);
        result.left   = std::max(left, inOther.left);
        result.right  = std::min(right, inOther.right);

        return result;
    }

    void Bounds2D::add(const Bounds2D& inBounds)
    {
        m_baseTop    = std::min(m_baseTop, inBounds.m_baseTop);
        m_baseLeft   = std::min(m_baseLeft, inBounds.m_baseLeft);
        m_baseBottom = std::max(m_baseBottom, inBounds.m_baseBottom);
        m_baseRight  = std::max(m_baseRight, inBounds.m_baseRight);
    }

    void Bounds2D::set(float inTop, float inLeft, float inBottom, float inRight)
    {
        m_baseTop    = inTop;
        m_baseLeft   = inLeft;
        m_baseBottom = inBottom;
        m_baseRight  = inRight;

        top    = inTop;
        left   = inLeft;
        bottom = inBottom;
        right  = inRight;
    }

    void Bounds2D::transform(const Mat3& inMatrix)
    {
        const glm::vec<3, float, glm::packed_highp> corners[4] = {
            inMatrix * glm::vec<3, float, glm::packed_highp>(m_baseLeft, m_baseTop, 1.0f),
            inMatrix * glm::vec<3, float, glm::packed_highp>(m_baseRight, m_baseTop, 1.0f),
            inMatrix * glm::vec<3, float, glm::packed_highp>(m_baseLeft, m_baseBottom, 1.0f),
            inMatrix * glm::vec<3, float, glm::packed_highp>(m_baseRight, m_baseBottom, 1.0f)
        };

        left   = corners[0].x;
        right  = corners[0].x;
        top    = corners[0].y;
        bottom = corners[0].y;

        for (int i = 1; i < 4; i++)
        {
            left   = std::min(left, corners[i].x);
            right  = std::max(right, corners[i].x);
            top    = std::min(top, corners[i].y);
            bottom = std::max(bottom, corners[i].y);
        }
    }
}