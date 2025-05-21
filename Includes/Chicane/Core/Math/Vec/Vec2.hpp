#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Math/Vec.hpp"

namespace Chicane
{
    struct CHICANE_CORE Vec2 : public Vec<2, float>
    {
    public:
        static const Vec2 Zero;

        static const Vec2 Up;
        static const Vec2 Right;

    public:
        template<typename... A>
        Vec2(A ...args)
            : Vec<2, float>(args...)
        {}

    public:
        inline friend bool operator==(const Vec2& inLeft, const Vec2& inRight)
        {
            return (
                std::fabs(inLeft.x - inRight.x) < FLT_EPSILON &&
                std::fabs(inLeft.y - inRight.y) < FLT_EPSILON
            );
        }
    };
}