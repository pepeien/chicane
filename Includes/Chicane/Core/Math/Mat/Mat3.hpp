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
            Mat3 result(0.0f);

            for (std::uint32_t column = 0; column < 3; column++)
            {
                for (std::uint32_t row = 0; row < 3; row++)
                {
                    result[column][row] = (inLeft[0][row] * inRight[column][0]) + (inLeft[1][row] * inRight[column][1]) +
                                          (inLeft[2][row] * inRight[column][2]);
                }
            }

            return result;
        }

        friend inline Vec3 operator*(const Mat3& inMatrix, const Vec3& inValue)
        {
            return Vec3(
                (inMatrix[0][0] * inValue.x) + (inMatrix[1][0] * inValue.y) + (inMatrix[2][0] * inValue.z),
                (inMatrix[0][1] * inValue.x) + (inMatrix[1][1] * inValue.y) + (inMatrix[2][1] * inValue.z),
                (inMatrix[0][2] * inValue.x) + (inMatrix[1][2] * inValue.y) + (inMatrix[2][2] * inValue.z)
            );
        }

    public:
        inline float determinant() const
        {
            const Vec3 column0((*this)[0][0], (*this)[0][1], (*this)[0][2]);
            const Vec3 column1((*this)[1][0], (*this)[1][1], (*this)[1][2]);
            const Vec3 column2((*this)[2][0], (*this)[2][1], (*this)[2][2]);

            return column0.dot(column1.cross(column2));
        }

        inline Mat3 inverse() const
        {
            const Vec3 column0((*this)[0][0], (*this)[0][1], (*this)[0][2]);
            const Vec3 column1((*this)[1][0], (*this)[1][1], (*this)[1][2]);
            const Vec3 column2((*this)[2][0], (*this)[2][1], (*this)[2][2]);
            const float inverseDeterminant = 1.0f / column0.dot(column1.cross(column2));
            const Vec3  row0               = column1.cross(column2) * inverseDeterminant;
            const Vec3  row1               = column2.cross(column0) * inverseDeterminant;
            const Vec3  row2               = column0.cross(column1) * inverseDeterminant;

            Mat3 result(1.0f);
            result[0][0] = row0.x;
            result[0][1] = row1.x;
            result[0][2] = row2.x;
            result[1][0] = row0.y;
            result[1][1] = row1.y;
            result[1][2] = row2.y;
            result[2][0] = row0.z;
            result[2][1] = row1.z;
            result[2][2] = row2.z;

            return result;
        }
    };
}
