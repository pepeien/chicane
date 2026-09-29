#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Quat.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"

namespace Chicane
{
    struct Mat3;
    struct Mat4;

    struct CHICANE_CORE QuatFloat : public Quat<float>
    {
    public:
        static QuatFloat sFromAxis(const Vec3& inAxis, float inAngle);
        static QuatFloat sFromEuler(const Vec3& inAngles);
        static QuatFloat sFromMatrix(const Mat3& inMatrix);
        static QuatFloat sLerp(const QuatFloat& inFrom, const QuatFloat& inTo, float inAmount);
        static QuatFloat sLookAt(const Vec3& inDirection, const Vec3& inUp);

    public:
        template <typename... A>
        constexpr QuatFloat(A... args)
            : Quat<float>(args...)
        {}

    public:
        friend inline QuatFloat operator*(const QuatFloat& inLeft, const QuatFloat& inRight)
        {
            return QuatFloat(
                (inLeft.w * inRight.w) - (inLeft.x * inRight.x) - (inLeft.y * inRight.y) - (inLeft.z * inRight.z),
                (inLeft.w * inRight.x) + (inLeft.x * inRight.w) + (inLeft.y * inRight.z) - (inLeft.z * inRight.y),
                (inLeft.w * inRight.y) + (inLeft.y * inRight.w) + (inLeft.z * inRight.x) - (inLeft.x * inRight.z),
                (inLeft.w * inRight.z) + (inLeft.z * inRight.w) + (inLeft.x * inRight.y) - (inLeft.y * inRight.x)
            );
        }

        friend inline Vec3 operator*(const QuatFloat& inOrientation, const Vec3& inValue)
        {
            const Vec3 axis(inOrientation.x, inOrientation.y, inOrientation.z);
            const Vec3 uv  = axis.cross(inValue);
            const Vec3 uuv = axis.cross(uv);

            return inValue + ((uv * inOrientation.w) + uuv) * 2.0f;
        }

    public:
        inline float dot(const QuatFloat& inValue) const
        {
            return (w * inValue.w) + (x * inValue.x) + (y * inValue.y) + (z * inValue.z);
        }

        inline QuatFloat inverse() const
        {
            const float scale = 1.0f / ((x * x) + (y * y) + (z * z) + (w * w));

            return QuatFloat(w * scale, -x * scale, -y * scale, -z * scale);
        }

        QuatFloat normalize() const;
        Vec3 toEuler() const;
        Mat4 toMatrix() const;
    };
}
