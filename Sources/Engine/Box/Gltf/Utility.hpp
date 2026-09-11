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
#include "Chicane/Core/Math/Mat/Mat4.hpp"
#include "Chicane/Core/Math/Quat/QuatFloat.hpp"
#include "Chicane/Core/Math/Transform.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"

#include "tiny_gltf_v3.h"

namespace Chicane
{
    namespace Box
    {
        inline const glm::mat3 BASIS(
            glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)
        );
        inline const glm::mat3 BASIS_INVERSE(
            glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)
        );
        inline const glm::mat4 BASIS4(
            glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f),
            glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
            glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
            glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
        );
        inline const glm::mat4 BASIS4_INVERSE(
            glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f),
            glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
            glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
            glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
        );

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
            const glm::quat source(inW, inX, inY, inZ);
            const glm::quat converted = glm::normalize(glm::quat_cast(BASIS * glm::mat3_cast(source) * BASIS_INVERSE));

            return QuatFloat(converted.w, converted.x, converted.y, converted.z);
        }

        inline Vec3 convertScale(float inX, float inY, float inZ)
        {
            return Vec3(inX, inZ, inY);
        }

        inline glm::mat4 toGlmMatrix(const double inMatrix[16])
        {
            return glm::mat4(
                glm::vec4(
                    static_cast<float>(inMatrix[0]),
                    static_cast<float>(inMatrix[1]),
                    static_cast<float>(inMatrix[2]),
                    static_cast<float>(inMatrix[3])
                ),
                glm::vec4(
                    static_cast<float>(inMatrix[4]),
                    static_cast<float>(inMatrix[5]),
                    static_cast<float>(inMatrix[6]),
                    static_cast<float>(inMatrix[7])
                ),
                glm::vec4(
                    static_cast<float>(inMatrix[8]),
                    static_cast<float>(inMatrix[9]),
                    static_cast<float>(inMatrix[10]),
                    static_cast<float>(inMatrix[11])
                ),
                glm::vec4(
                    static_cast<float>(inMatrix[12]),
                    static_cast<float>(inMatrix[13]),
                    static_cast<float>(inMatrix[14]),
                    static_cast<float>(inMatrix[15])
                )
            );
        }

        inline Transform transformFromMatrix(const glm::mat4& inMatrix)
        {
            const glm::vec3 column0 = glm::vec3(inMatrix[0]);
            const glm::vec3 column1 = glm::vec3(inMatrix[1]);
            const glm::vec3 column2 = glm::vec3(inMatrix[2]);

            const float scaleX = glm::length(column0);
            const float scaleY = glm::length(column1);
            const float scaleZ = glm::length(column2);

            glm::mat3 rotation(1.0f);
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

            if (glm::determinant(rotation) < 0.0f)
            {
                rotation[0] *= -1.0f;
            }

            const glm::quat orientation = glm::normalize(glm::quat_cast(rotation));

            Transform transform;
            transform.setTranslation(Vec3(inMatrix[3].x, inMatrix[3].y, inMatrix[3].z));
            transform.setRotation(QuatFloat(orientation.w, orientation.x, orientation.y, orientation.z));
            transform.setScale(Vec3(scaleX, scaleY, scaleZ));

            return transform;
        }

        inline glm::mat4 localGltfMatrix(const tg3_node& inNode)
        {
            if (inNode.has_matrix)
            {
                return toGlmMatrix(inNode.matrix);
            }

            const glm::vec3 translation(
                static_cast<float>(inNode.translation[0]),
                static_cast<float>(inNode.translation[1]),
                static_cast<float>(inNode.translation[2])
            );
            const glm::quat rotation(
                static_cast<float>(inNode.rotation[3]),
                static_cast<float>(inNode.rotation[0]),
                static_cast<float>(inNode.rotation[1]),
                static_cast<float>(inNode.rotation[2])
            );
            const glm::vec3 scale(
                static_cast<float>(inNode.scale[0]),
                static_cast<float>(inNode.scale[1]),
                static_cast<float>(inNode.scale[2])
            );

            glm::mat4 matrix(1.0f);
            matrix = glm::translate(matrix, translation);
            matrix *= glm::mat4_cast(rotation);
            matrix = glm::scale(matrix, scale);

            return matrix;
        }

        inline Transform convertTransform(const tg3_node& inNode)
        {
            if (inNode.has_matrix)
            {
                return transformFromMatrix(BASIS4 * toGlmMatrix(inNode.matrix) * BASIS4_INVERSE);
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
