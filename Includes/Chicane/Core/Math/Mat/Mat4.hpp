#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Mat.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"

namespace Chicane
{
    struct CHICANE_CORE Mat4 : public Mat<4, float>
    {
    public:
        static constexpr inline const Mat<4, float> Zero = Mat<4, float>(0.0f);
        static constexpr inline const Mat<4, float> One  = Mat<4, float>(1.0f);

    public:
        static bool toPosition(
            const Vec3& inWorldPosition,
            const Mat4& inView,
            const Mat4& inProjection,
            const Vec2& inViewport,
            Vec2&       outPosition
        );
        static Vec2 toPosition(
            const Vec3& inWorldPosition, const Mat4& inView, const Mat4& inProjection, const Vec2& inViewport
        );

    public:
        template <typename... A>
        constexpr Mat4(A... args)
            : Mat<4, float>(args...)
        {}

    public:
        friend inline Mat4 operator*(const Mat4& inLeft, const Mat4& inRight)
        {
            return static_cast<const glm::mat4&>(inLeft) * static_cast<const glm::mat4&>(inRight);
        }

        friend inline Vec4 operator*(const Mat4& inMatrix, const Vec4& inValue)
        {
            const glm::vec4 mapped = static_cast<const glm::mat4&>(inMatrix) * static_cast<glm::vec4>(inValue);

            return Vec4(mapped);
        }

        friend inline Vec3 operator*(const Mat4& inMatrix, const Vec3& inValue)
        {
            const glm::vec4 mapped =
                static_cast<const glm::mat4&>(inMatrix) * glm::vec4(inValue.x, inValue.y, inValue.z, 1.0f);

            return Vec3(mapped);
        }

    public:
        Vec3 getTranslation() const;
        Mat4 inverse() const;

        bool toPosition(const Mat4& inView, const Mat4& inProjection, const Vec2& inViewport, Vec2& outPosition) const;
        Vec2 toPosition(const Mat4& inView, const Mat4& inProjection, const Vec2& inViewport) const;

        static bool fromPosition(
            const Vec2& inPosition,
            const Mat4& inView,
            const Mat4& inProjection,
            const Vec2& inViewport,
            Vec3&       outNear,
            Vec3&       outFar
        );
    };
}
