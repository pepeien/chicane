#include "Chicane/Core/Math/Quat/QuatFloat.hpp"

#include <cmath>
#include <limits>

#include "Chicane/Core/Math/Mat/Mat3.hpp"
#include "Chicane/Core/Math/Mat/Mat4.hpp"

namespace Chicane
{
    static constexpr float DEGREES_TO_RADIANS = 0.01745329251994329576923690768489f;

    QuatFloat QuatFloat::sFromAxis(const Vec3& inAxis, float inAngle)
    {
        const Vec3  axis = inAxis.normalize();
        const float half = inAngle * 0.5f;
        const float sine = std::sin(half);

        return QuatFloat(std::cos(half), axis.x * sine, axis.y * sine, axis.z * sine);
    }

    QuatFloat QuatFloat::sFromEuler(const Vec3& inAngles)
    {
        const float x = inAngles.x * DEGREES_TO_RADIANS * 0.5f;
        const float y = inAngles.y * DEGREES_TO_RADIANS * 0.5f;
        const float z = inAngles.z * DEGREES_TO_RADIANS * 0.5f;

        const float cosineX = std::cos(x);
        const float sineX   = std::sin(x);
        const float cosineY = std::cos(y);
        const float sineY   = std::sin(y);
        const float cosineZ = std::cos(z);
        const float sineZ   = std::sin(z);

        return QuatFloat(
            (cosineX * cosineY * cosineZ) + (sineX * sineY * sineZ),
            (sineX * cosineY * cosineZ) - (cosineX * sineY * sineZ),
            (cosineX * sineY * cosineZ) + (sineX * cosineY * sineZ),
            (cosineX * cosineY * sineZ) - (sineX * sineY * cosineZ)
        );
    }

    QuatFloat QuatFloat::sFromMatrix(const Mat3& inMatrix)
    {
        const float fourX = inMatrix[0][0] - inMatrix[1][1] - inMatrix[2][2];
        const float fourY = inMatrix[1][1] - inMatrix[0][0] - inMatrix[2][2];
        const float fourZ = inMatrix[2][2] - inMatrix[0][0] - inMatrix[1][1];
        const float fourW = inMatrix[0][0] + inMatrix[1][1] + inMatrix[2][2];

        int   biggest = 0;
        float value   = fourW;

        if (fourX > value)
        {
            value   = fourX;
            biggest = 1;
        }

        if (fourY > value)
        {
            value   = fourY;
            biggest = 2;
        }

        if (fourZ > value)
        {
            value   = fourZ;
            biggest = 3;
        }

        const float biggestValue = std::sqrt(value + 1.0f) * 0.5f;
        const float multiplier   = 0.25f / biggestValue;

        switch (biggest)
        {
        case 0:
            return QuatFloat(
                biggestValue,
                (inMatrix[1][2] - inMatrix[2][1]) * multiplier,
                (inMatrix[2][0] - inMatrix[0][2]) * multiplier,
                (inMatrix[0][1] - inMatrix[1][0]) * multiplier
            );

        case 1:
            return QuatFloat(
                (inMatrix[1][2] - inMatrix[2][1]) * multiplier,
                biggestValue,
                (inMatrix[0][1] + inMatrix[1][0]) * multiplier,
                (inMatrix[2][0] + inMatrix[0][2]) * multiplier
            );

        case 2:
            return QuatFloat(
                (inMatrix[2][0] - inMatrix[0][2]) * multiplier,
                (inMatrix[0][1] + inMatrix[1][0]) * multiplier,
                biggestValue,
                (inMatrix[1][2] + inMatrix[2][1]) * multiplier
            );

        default:
            return QuatFloat(
                (inMatrix[0][1] - inMatrix[1][0]) * multiplier,
                (inMatrix[2][0] + inMatrix[0][2]) * multiplier,
                (inMatrix[1][2] + inMatrix[2][1]) * multiplier,
                biggestValue
            );
        }
    }

    QuatFloat QuatFloat::sLerp(const QuatFloat& inFrom, const QuatFloat& inTo, float inAmount)
    {
        QuatFloat to     = inTo;
        float     cosine = inFrom.dot(to);

        if (cosine < 0.0f)
        {
            to     = QuatFloat(-to.w, -to.x, -to.y, -to.z);
            cosine = -cosine;
        }

        if (cosine > 1.0f - std::numeric_limits<float>::epsilon())
        {
            return QuatFloat(
                inFrom.w + ((to.w - inFrom.w) * inAmount),
                inFrom.x + ((to.x - inFrom.x) * inAmount),
                inFrom.y + ((to.y - inFrom.y) * inAmount),
                inFrom.z + ((to.z - inFrom.z) * inAmount)
            );
        }

        const float angle      = std::acos(cosine);
        const float inverse    = 1.0f / std::sin(angle);
        const float fromWeight = std::sin((1.0f - inAmount) * angle) * inverse;
        const float toWeight   = std::sin(inAmount * angle) * inverse;

        return QuatFloat(
            (inFrom.w * fromWeight) + (to.w * toWeight),
            (inFrom.x * fromWeight) + (to.x * toWeight),
            (inFrom.y * fromWeight) + (to.y * toWeight),
            (inFrom.z * fromWeight) + (to.z * toWeight)
        );
    }

    QuatFloat QuatFloat::sLookAt(const Vec3& inDirection, const Vec3& inUp)
    {
        return glm::quatLookAt(static_cast<glm::vec3>(inDirection), static_cast<glm::vec3>(inUp));
    }

    QuatFloat QuatFloat::normalize() const
    {
        const float length = std::sqrt((x * x) + (y * y) + (z * z) + (w * w));

        if (length <= 0.0f)
        {
            return QuatFloat(1.0f, 0.0f, 0.0f, 0.0f);
        }

        const float inverse = 1.0f / length;

        return QuatFloat(w * inverse, x * inverse, y * inverse, z * inverse);
    }

    Vec3 QuatFloat::toEuler() const
    {
        return glm::degrees(glm::eulerAngles(static_cast<const glm::quat&>(*this)));
    }

    Mat4 QuatFloat::toMatrix() const
    {
        const float qxx = x * x;
        const float qyy = y * y;
        const float qzz = z * z;
        const float qxz = x * z;
        const float qxy = x * y;
        const float qyz = y * z;
        const float qwx = w * x;
        const float qwy = w * y;
        const float qwz = w * z;

        Mat4 result(1.0f);

        result[0][0] = 1.0f - (2.0f * (qyy + qzz));
        result[0][1] = 2.0f * (qxy + qwz);
        result[0][2] = 2.0f * (qxz - qwy);

        result[1][0] = 2.0f * (qxy - qwz);
        result[1][1] = 1.0f - (2.0f * (qxx + qzz));
        result[1][2] = 2.0f * (qyz + qwx);

        result[2][0] = 2.0f * (qxz + qwy);
        result[2][1] = 2.0f * (qyz - qwx);
        result[2][2] = 1.0f - (2.0f * (qxx + qyy));

        return result;
    }
}
