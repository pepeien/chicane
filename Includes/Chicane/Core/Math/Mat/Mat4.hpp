#pragma once

#include <cmath>

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
        static bool sToPosition(
            const Vec3& inWorldPosition,
            const Mat4& inView,
            const Mat4& inProjection,
            const Vec2& inViewport,
            Vec2&       outPosition
        );
        static Vec2 sToPosition(
            const Vec3& inWorldPosition, const Mat4& inView, const Mat4& inProjection, const Vec2& inViewport
        );

    public:
        template <typename... A>
        constexpr Mat4(A... args)
            : Mat<4, float>(args...)
        {}

        static inline Mat4 sTranslate(const Vec3& inTranslation)
        {
            Mat4 result(1.0f);
            result[3][0] = inTranslation.x;
            result[3][1] = inTranslation.y;
            result[3][2] = inTranslation.z;

            return result;
        }

        static inline Mat4 sScale(const Vec3& inScale)
        {
            Mat4 result(1.0f);
            result[0][0] = inScale.x;
            result[1][1] = inScale.y;
            result[2][2] = inScale.z;

            return result;
        }

        static inline Mat4 sOrtho(float inLeft, float inRight, float inBottom, float inTop, float inNear, float inFar)
        {
            Mat4 result(1.0f);
            result[0][0] = 2.0f / (inRight - inLeft);
            result[1][1] = 2.0f / (inTop - inBottom);
            result[2][2] = -2.0f / (inFar - inNear);
            result[3][0] = -(inRight + inLeft) / (inRight - inLeft);
            result[3][1] = -(inTop + inBottom) / (inTop - inBottom);
            result[3][2] = -(inFar + inNear) / (inFar - inNear);

            return result;
        }

        static inline Mat4 sPerspective(float inFieldOfView, float inAspect, float inNear, float inFar)
        {
            const float half = std::tan(inFieldOfView * 0.5f);

            Mat4        result(0.0f);
            result[0][0] = 1.0f / (inAspect * half);
            result[1][1] = 1.0f / half;
            result[2][2] = -(inFar + inNear) / (inFar - inNear);
            result[2][3] = -1.0f;
            result[3][2] = -(2.0f * inFar * inNear) / (inFar - inNear);

            return result;
        }

        static inline Mat4 sLookAt(const Vec3& inEye, const Vec3& inCenter, const Vec3& inUp)
        {
            const Vec3 forward = (inCenter - inEye).normalize();
            const Vec3 side    = forward.cross(inUp).normalize();
            const Vec3 up      = side.cross(forward);

            Mat4       result(1.0f);
            result[0][0] = side.x;
            result[1][0] = side.y;
            result[2][0] = side.z;
            result[0][1] = up.x;
            result[1][1] = up.y;
            result[2][1] = up.z;
            result[0][2] = -forward.x;
            result[1][2] = -forward.y;
            result[2][2] = -forward.z;
            result[3][0] = -side.dot(inEye);
            result[3][1] = -up.dot(inEye);
            result[3][2] = forward.dot(inEye);

            return result;
        }

    public:
        friend inline Mat4 operator*(const Mat4& inLeft, const Mat4& inRight)
        {
            Mat4 result(0.0f);

            for (std::uint32_t column = 0; column < 4; column++)
            {
                for (std::uint32_t row = 0; row < 4; row++)
                {
                    result[column][row] = (inLeft[0][row] * inRight[column][0]) +
                                          (inLeft[1][row] * inRight[column][1]) +
                                          (inLeft[2][row] * inRight[column][2]) + (inLeft[3][row] * inRight[column][3]);
                }
            }

            return result;
        }

        friend inline Vec4 operator*(const Mat4& inMatrix, const Vec4& inValue)
        {
            return Vec4(
                (inMatrix[0][0] * inValue.x) + (inMatrix[1][0] * inValue.y) + (inMatrix[2][0] * inValue.z) +
                    (inMatrix[3][0] * inValue.w),
                (inMatrix[0][1] * inValue.x) + (inMatrix[1][1] * inValue.y) + (inMatrix[2][1] * inValue.z) +
                    (inMatrix[3][1] * inValue.w),
                (inMatrix[0][2] * inValue.x) + (inMatrix[1][2] * inValue.y) + (inMatrix[2][2] * inValue.z) +
                    (inMatrix[3][2] * inValue.w),
                (inMatrix[0][3] * inValue.x) + (inMatrix[1][3] * inValue.y) + (inMatrix[2][3] * inValue.z) +
                    (inMatrix[3][3] * inValue.w)
            );
        }

        friend inline Vec3 operator*(const Mat4& inMatrix, const Vec3& inValue)
        {
            return Vec3(
                (inMatrix[0][0] * inValue.x) + (inMatrix[1][0] * inValue.y) + (inMatrix[2][0] * inValue.z) +
                    inMatrix[3][0],
                (inMatrix[0][1] * inValue.x) + (inMatrix[1][1] * inValue.y) + (inMatrix[2][1] * inValue.z) +
                    inMatrix[3][1],
                (inMatrix[0][2] * inValue.x) + (inMatrix[1][2] * inValue.y) + (inMatrix[2][2] * inValue.z) +
                    inMatrix[3][2]
            );
        }

    public:
        Vec3 getTranslation() const;
        Mat4 inverse() const;

        bool toPosition(const Mat4& inView, const Mat4& inProjection, const Vec2& inViewport, Vec2& outPosition) const;
        Vec2 toPosition(const Mat4& inView, const Mat4& inProjection, const Vec2& inViewport) const;

        static bool sFromPosition(
            const Vec2& inPosition,
            const Mat4& inView,
            const Mat4& inProjection,
            const Vec2& inViewport,
            Vec3&       outNear,
            Vec3&       outFar
        );
    };
}
