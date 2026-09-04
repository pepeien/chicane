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
        static QuatFloat fromEuler(const Vec3& inAngles);
        static QuatFloat lookAt(const Vec3& inDirection, const Vec3& inUp);

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

        friend inline Vec3 operator*(const QuatFloat& inOrientation, const Vec3& inValue)
        {
            const glm::vec3 mapped =
                static_cast<const glm::quat&>(inOrientation) * static_cast<glm::vec3>(inValue);

            return Vec3(mapped);
        }

    public:
        QuatFloat normalize() const;
        Vec3      toEuler() const;
    };
}
