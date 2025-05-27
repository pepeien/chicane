#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Math/Vec/Base.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"

namespace Chicane
{
    struct CHICANE_CORE Vec4 : public Vec<4, float>
    {
    public:
        static inline constexpr const Vec<4, float> Zero = Vec<4, float>(0.0f);
        static inline constexpr const Vec<4, float> One  = Vec<4, float>(1.0f);

        static inline constexpr const Vec<4, float> Up      = Vec<4, float>(Vec3::Up,      0.0f);
        static inline constexpr const Vec<4, float> Right   = Vec<4, float>(Vec3::Right,   0.0f);
        static inline constexpr const Vec<4, float> Forward = Vec<4, float>(Vec3::Forward, 0.0f);

    public:
        template<typename... A>
        constexpr Vec4(A ...args)
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