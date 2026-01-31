#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    struct CHICANE_CORE Vec2 : public Vec<2, float>
    {
    public:
        static inline constexpr const Vec<2, float> Zero = Vec<2, float>(0.0f);
        static inline constexpr const Vec<2, float> One  = Vec<2, float>(1.0f);

        static inline constexpr const Vec<2, float> Up    = Vec<2, float>(Vec3::Up.x, Vec3::Up.y);
        static inline constexpr const Vec<2, float> Right = Vec<2, float>(Vec3::Right.x, Vec3::Right.y);

    public:
        template <typename... A>
        constexpr Vec2(A... args)
            : Vec<2, float>(args...)
        {}

    public:
        inline friend bool operator==(const Vec2& inLeft, const Vec2& inRight)
        {
            return std::fabs(inLeft.x - inRight.x) < FLT_EPSILON && std::fabs(inLeft.y - inRight.y) < FLT_EPSILON;
        }

        inline operator String() const { return toString(); }

        inline String toString() const { return String::sprint("[%.2f, %.2f]", x, y); }
    };
}