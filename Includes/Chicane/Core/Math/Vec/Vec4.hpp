#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Math/Vec.hpp"

namespace Chicane
{
    struct CHICANE_CORE Vec4 : public Vec<4, float>
    {
    public:
        static const Vec4 Zero;

        static const Vec4 Up;
        static const Vec4 Right;
        static const Vec4 Forward;

    public:
        template<typename... A>
        Vec4(A ...args)
            : Vec<4, float>(args...)
        {}

    public:
        inline friend bool operator==(const Vec4& inLeft, const Vec4& inRight)
        {
            return (
                std::fabs(inLeft.x - inRight.x) < FLT_EPSILON &&
                std::fabs(inLeft.y - inRight.y) < FLT_EPSILON &&
                std::fabs(inLeft.z - inRight.z) < FLT_EPSILON &&
                std::fabs(inLeft.w - inRight.w) < FLT_EPSILON
            );
        }
    };
}