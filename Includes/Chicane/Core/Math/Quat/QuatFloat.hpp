#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Quat.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"

namespace Chicane
{
    struct CHICANE_CORE QuatFloat : public Quat<float>
    {
    public:
        static QuatFloat fromAxis(const Vec3& inAxis, float inAngle);

    public:
        template <typename... A>
        constexpr QuatFloat(A... args)
            : Quat<float>(args...)
        {}

    public:
        friend inline QuatFloat operator*(const QuatFloat& inLeft, const QuatFloat& inRight)
        {
            return static_cast<const glm::quat&>(inLeft) * static_cast<const glm::quat&>(inRight);
        }

    public:
        QuatFloat normalize() const;
    };
}
