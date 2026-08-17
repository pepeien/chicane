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