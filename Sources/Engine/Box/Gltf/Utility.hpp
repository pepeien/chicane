#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Chicane/Box/Model/Raw.hpp"
#include "Chicane/Box/Skeleton/Bone.hpp"

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Math/Mat/Mat3.hpp"
#include "Chicane/Core/Math/Mat/Mat4.hpp"
#include "Chicane/Core/Math/Quat/QuatFloat.hpp"
#include "Chicane/Core/Math/Transform.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"

#include "tiny_gltf_v3.h"

namespace Chicane
{
    namespace Box
    {
        inline const Mat3 BASIS(Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f));
        inline const Mat3 BASIS_INVERSE(Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f));
        inline const Mat4 BASIS4(
            Vec4(-1.0f, 0.0f, 0.0f, 0.0f),
            Vec4(0.0f, 0.0f, 1.0f, 0.0f),
            Vec4(0.0f, 1.0f, 0.0f, 0.0f),
            Vec4(0.0f, 0.0f, 0.0f, 1.0f)
        );
        inline const Mat4 BASIS4_INVERSE(
            Vec4(-1.0f, 0.0f, 0.0f, 0.0f),
            Vec4(0.0f, 0.0f, 1.0f, 0.0f),
            Vec4(0.0f, 1.0f, 0.0f, 0.0f),
            Vec4(0.0f, 0.0f, 0.0f, 1.0f)
        );

        inline Mat3 rotationMatrix(const QuatFloat& inValue)
        {
            const Mat4 matrix = inValue.toMatrix();
            Mat3       result(1.0f);
            result[0] = Vec3(matrix[0]);
            result[1] = Vec3(matrix[1]);
            result[2] = Vec3(matrix[2]);

            return result;
        }

        inline String toString(const tg3_str& inValue)
        {
            if (!inValue.data || inValue.len == 0)
            {
                return "";
            }

            return String(inValue.data, inValue.data + inValue.len);
        }

        inline bool equals(const tg3_str& inValue, const char* inOther)
        {
            return tg3_str_equals_cstr(inValue, inOther) != 0;
        }

        inline Vec3 convertVector(float inX, float inY, float inZ)
        {
            return Vec3(-inX, inZ, inY);
        }

        inline QuatFloat convertRotation(float inX, float inY, float inZ, float inW)
        {
            const QuatFloat source(inW, inX, inY, inZ);

            return QuatFloat::sFromMatrix(BASIS * rotationMatrix(source) * BASIS_INVERSE).normalize();
        }

        inline Vec3 convertScale(float inX, float inY, float inZ)
        {
            return Vec3(inX, inZ, inY);
        }

        inline Mat4 toMatrix(const double inMatrix[16])
        {
            return Mat4(
                Vec4(
                    static_cast<float>(inMatrix[0]),
                    static_cast<float>(inMatrix[1]),
                    static_cast<float>(inMatrix[2]),
                    static_cast<float>(inMatrix[3])
                ),
                Vec4(
                    static_cast<float>(inMatrix[4]),
                    static_cast<float>(inMatrix[5]),
                    static_cast<float>(inMatrix[6]),
                    static_cast<float>(inMatrix[7])
                ),
                Vec4(
                    static_cast<float>(inMatrix[8]),
                    static_cast<float>(inMatrix[9]),
                    static_cast<float>(inMatrix[10]),
                    static_cast<float>(inMatrix[11])
                ),
                Vec4(
                    static_cast<float>(inMatrix[12]),
                    static_cast<float>(inMatrix[13]),
                    static_cast<float>(inMatrix[14]),
                    static_cast<float>(inMatrix[15])
                )
            );
        }

        inline Transform transformFromMatrix(const Mat4& inMatrix)
        {
            const Vec3 column0(inMatrix[0]);
            const Vec3 column1(inMatrix[1]);
            const Vec3 column2(inMatrix[2]);

            const float scaleX = column0.length();
            const float scaleY = column1.length();
            const float scaleZ = column2.length();

            Mat3 rotation(1.0f);
            if (scaleX > 1e-8f)
            {
                rotation[0] = column0 / scaleX;
            }

            if (scaleY > 1e-8f)
            {
                rotation[1] = column1 / scaleY;
            }

            if (scaleZ > 1e-8f)
            {
                rotation[2] = column2 / scaleZ;
            }

            if (rotation.determinant() < 0.0f)
            {
                rotation[0] *= -1.0f;
            }

            Transform transform;
            transform.setTranslation(Vec3(inMatrix[3].x, inMatrix[3].y, inMatrix[3].z));
            transform.setRotation(QuatFloat::sFromMatrix(rotation).normalize());
            transform.setScale(Vec3(scaleX, scaleY, scaleZ));

            return transform;
        }

        inline Mat4 localGltfMatrix(const tg3_node& inNode)
        {
            if (inNode.has_matrix)
            {
                return toMatrix(inNode.matrix);
            }

            const Vec3 translation(
                static_cast<float>(inNode.translation[0]),
                static_cast<float>(inNode.translation[1]),
                static_cast<float>(inNode.translation[2])
            );
            const QuatFloat rotation(
                static_cast<float>(inNode.rotation[3]),
                static_cast<float>(inNode.rotation[0]),
                static_cast<float>(inNode.rotation[1]),
                static_cast<float>(inNode.rotation[2])
            );
            const Vec3 scale(
                static_cast<float>(inNode.scale[0]),
                static_cast<float>(inNode.scale[1]),
                static_cast<float>(inNode.scale[2])
            );

            return Mat4::sTranslate(translation) * rotation.toMatrix() * Mat4::sScale(scale);
        }

        inline Transform convertTransform(const tg3_node& inNode)
        {
            if (inNode.has_matrix)
            {
                return transformFromMatrix(BASIS4 * toMatrix(inNode.matrix) * BASIS4_INVERSE);
            }

            Transform transform;
            transform.setTranslation(convertVector(
                static_cast<float>(inNode.translation[0]),
                static_cast<float>(inNode.translation[1]),
                static_cast<float>(inNode.translation[2])
            ));
            transform.setRotation(convertRotation(
                static_cast<float>(inNode.rotation[0]),
                static_cast<float>(inNode.rotation[1]),
                static_cast<float>(inNode.rotation[2]),
                static_cast<float>(inNode.rotation[3])
            ));
            transform.setScale(convertScale(
                static_cast<float>(inNode.scale[0]),
                static_cast<float>(inNode.scale[1]),
                static_cast<float>(inNode.scale[2])
            ));

            return transform;
        }

    }
}
