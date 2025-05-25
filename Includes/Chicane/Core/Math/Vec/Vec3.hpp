#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Math/Vec/Base.hpp"

namespace Chicane
{
    struct CHICANE_CORE Vec3 : public Vec<3, float>
    {
    public:
        static const Vec3 Zero;
        static const Vec3 One;

        static const Vec3 Up;
        static const Vec3 Right;
        static const Vec3 Forward;

    public:
        template<typename... A>
        constexpr Vec3(A ...args)
            : Vec<3, float>(args...)
        {}

    public:
        inline friend bool operator==(const Vec3& inLeft, const Vec3& inRight)
        {
            return (
                std::fabs(inLeft.x - inRight.x) < FLT_EPSILON &&
                std::fabs(inLeft.y - inRight.y) < FLT_EPSILON &&
                std::fabs(inLeft.z - inRight.z) < FLT_EPSILON
            );
        }
    };
}