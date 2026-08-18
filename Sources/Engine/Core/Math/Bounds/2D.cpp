#include "Chicane/Core/Math/Bounds/2D.hpp"

#include <algorithm>

namespace Chicane
{
    Bounds2D::Bounds2D()
        : top(0.0f),
          bottom(0.0f),
          left(0.0f),
          right(0.0f)
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
}