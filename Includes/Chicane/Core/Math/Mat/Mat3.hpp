#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Mat.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"

namespace Chicane
{
    struct CHICANE_CORE Mat3 : public Mat<3, float>
    {
    public:
        static constexpr inline const Mat<3, float> Zero = Mat<3, float>(0.0f);
        static constexpr inline const Mat<3, float> One  = Mat<3, float>(1.0f);

    public:
        template <typename... A>
        constexpr Mat3(A... args)
            : Mat<3, float>(args...)
        {}

    public:
        friend inline Mat3 operator*(const Mat3& inLeft, const Mat3& inRight)
        {
            return static_cast<const glm::mat3&>(inLeft) * static_cast<const glm::mat3&>(inRight);
        }

        friend inline Vec3 operator*(const Mat3& inMatrix, const Vec3& inValue)
        {
            const glm::vec3 mapped = static_cast<const glm::mat3&>(inMatrix) * static_cast<glm::vec3>(inValue);

            return Vec3(mapped);
        }
    };
}
