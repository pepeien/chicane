#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Vec.hpp"

namespace Chicane
{
    struct CHICANE_CORE Vec3 : public Vec<3, float>
    {
    public:
        static inline constexpr const Vec<3, float> Zero    = Vec<3, float>(0.0f);
        static inline constexpr const Vec<3, float> One     = Vec<3, float>(1.0f);

        static inline constexpr const Vec<3, float> Right   = Vec<3, float>(1.0f, 0.0f, 0.0f);
        static inline constexpr const Vec<3, float> Forward = Vec<3, float>(0.0f, 1.0f, 0.0f);
        static inline constexpr const Vec<3, float> Up      = Vec<3, float>(0.0f, 0.0f, 1.0f);

    public:
        template <typename... A>
        constexpr Vec3(A... args)
            : Vec<3, float>(args...)
        {}

    public:
        inline friend bool operator==(const Vec3& inLeft, const Vec3& inRight)
        {
            return (
                std::fabs(inLeft.x - inRight.x) < FLT_EPSILON && std::fabs(inLeft.y - inRight.y) < FLT_EPSILON &&
                std::fabs(inLeft.z - inRight.z) < FLT_EPSILON
            );
        }
    };
}