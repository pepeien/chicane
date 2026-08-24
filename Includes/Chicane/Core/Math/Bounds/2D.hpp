#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Mat/Mat3.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"

namespace Chicane
{
    struct CHICANE_CORE Bounds2D
    {
    public:
        Bounds2D();

    public:
        static inline const Bounds2D& unconstrained()
        {
            static Bounds2D result = {};
            result.top             = -1.0e9f;
            result.bottom          = 1.0e9f;
            result.left            = -1.0e9f;
            result.right           = 1.0e9f;

            return result;
        }

    public:
        bool contains(const Vec2& inPoint) const;
        bool containsRounded(const Vec2& inPoint, const Vec4& inHorizontalRadius, const Vec4& inVerticalRadius) const;

        bool overlaps(const Bounds2D& inOther) const;

        bool isEmpty() const;

        Bounds2D intersect(const Bounds2D& inOther) const;

        void add(const Bounds2D& inBounds);
        void set(float inTop, float inLeft, float inBottom, float inRight);
        void transform(const Mat3& inMatrix);

    public:
        float top;
        float bottom;
        float left;
        float right;

    private:
        float m_baseTop;
        float m_baseBottom;
        float m_baseLeft;
        float m_baseRight;
    };
}