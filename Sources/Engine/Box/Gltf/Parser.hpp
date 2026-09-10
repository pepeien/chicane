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
        namespace
        {
            const glm::mat3 BASIS(
                glm::vec3(-1.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 1.0f),
                glm::vec3(0.0f, 1.0f, 0.0f)
            );
            const glm::mat3 BASIS_INVERSE(
                glm::vec3(-1.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 1.0f),
                glm::vec3(0.0f, 1.0f, 0.0f)
            );
            const glm::mat4 BASIS4(
                glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f),
                glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
                glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
                glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
            );
            const glm::mat4 BASIS4_INVERSE(
                glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f),
                glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
                glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
                glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
            );

            String toString(const tg3_str& inValue)
            {
                if (!inValue.data || inValue.len == 0)
                {
                    return "";
                }

                return String(inValue.data, inValue.data + inValue.len);
            }

            bool equals(const tg3_str& inValue, const char* inOther)
            {
                return tg3_str_equals_cstr(inValue, inOther) != 0;
            }

            Vec3 convertVector(float inX, float inY, float inZ)
            {
                return Vec3(-inX, inZ, inY);
            }

            QuatFloat convertRotation(float inX, float inY, float inZ, float inW)
            {
                const glm::quat source(inW, inX, inY, inZ);
                const glm::quat converted =
                    glm::normalize(glm::quat_cast(BASIS * glm::mat3_cast(source) * BASIS_INVERSE));

                return QuatFloat(converted.w, converted.x, converted.y, converted.z);
            }

            Vec3 convertScale(float inX, float inY, float inZ)
            {
                return Vec3(inX, inZ, inY);
            }

            glm::mat4 toGlmMatrix(const double inMatrix[16])
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

            Transform transformFromMatrix(const glm::mat4& inMatrix)
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

            glm::mat4 localGltfMatrix(const tg3_node& inNode)
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
                matrix  = glm::scale(matrix, scale);

                return matrix;
            }

            Transform convertTransform(const tg3_node& inNode)
            {
                if (inNode.has_matrix)
                {
                    return transformFromMatrix(BASIS4 * toGlmMatrix(inNode.matrix) * BASIS4_INVERSE);
                }

                Transform transform;
                transform.setTranslation(
                    convertVector(
                        static_cast<float>(inNode.translation[0]),
                        static_cast<float>(inNode.translation[1]),
                        static_cast<float>(inNode.translation[2])
                    )
                );
                transform.setRotation(
                    convertRotation(
                        static_cast<float>(inNode.rotation[0]),
                        static_cast<float>(inNode.rotation[1]),
                        static_cast<float>(inNode.rotation[2]),
                        static_cast<float>(inNode.rotation[3])
                    )
                );
                transform.setScale(
                    convertScale(
                        static_cast<float>(inNode.scale[0]),
                        static_cast<float>(inNode.scale[1]),
                        static_cast<float>(inNode.scale[2])
                    )
                );

                return transform;
            }

            class Document
            {
            public:
                Document(const ModelRaw& inData, const String& inBaseDirectory = "", const String& inLabel = "glTF")
                {
                    parse(inData, inBaseDirectory, inLabel);
                }

                Document(const FileSystem::Path& inPath)
                {
                    if (!FileSystem::exists(inPath))
                    {
                        throw std::runtime_error("The glTF file [" + inPath.toString().toStandard() + "] doesn't exist");
                    }

                    parse(FileSystem::readUnsigned(inPath), inPath.parent().toString(), inPath.toString());
                }

                const tg3_model& get() const
                {
                    return *m_model.get();
                }

                String nodeName(std::int32_t inIndex) const
                {
                    const tg3_model& model = get();
                    if (inIndex < 0 || static_cast<std::uint32_t>(inIndex) >= model.nodes_count)
                    {
                        return String::sprint("Node_%d", inIndex);
                    }

                    const String name = toString(model.nodes[inIndex].name);
                    if (!name.isEmpty())
                    {
                        return name;
                    }

                    return String::sprint("Node_%d", inIndex);
                }

                std::vector<std::int32_t> parents() const
                {
                    const tg3_model&          model = get();
                    std::vector<std::int32_t> result(model.nodes_count, -1);

                    for (std::uint32_t i = 0; i < model.nodes_count; ++i)
                    {
                        const tg3_node& node = model.nodes[i];
                        for (std::uint32_t c = 0; c < node.children_count; ++c)
                        {
                            const std::int32_t child = node.children[c];
                            if (child < 0 || static_cast<std::uint32_t>(child) >= model.nodes_count)
                            {
                                continue;
                            }

                            result[static_cast<std::size_t>(child)] = static_cast<std::int32_t>(i);
                        }
                    }

                    return result;
                }

                const uint8_t* accessorBytes(std::int32_t inAccessor, std::int32_t& outStride) const
                {
                    const tg3_model& model = get();
                    if (inAccessor < 0 || static_cast<std::uint32_t>(inAccessor) >= model.accessors_count)
                    {
                        return nullptr;
                    }

                    const tg3_accessor& accessor = model.accessors[inAccessor];
                    if (accessor.buffer_view < 0 ||
                        static_cast<std::uint32_t>(accessor.buffer_view) >= model.buffer_views_count)
                    {
                        return nullptr;
                    }

                    const tg3_buffer_view& view = model.buffer_views[accessor.buffer_view];
                    if (view.buffer < 0 || static_cast<std::uint32_t>(view.buffer) >= model.buffers_count)
                    {
                        return nullptr;
                    }

                    const tg3_buffer& buffer = model.buffers[view.buffer];
                    if (!buffer.data.data)
                    {
                        return nullptr;
                    }

                    outStride = tg3_accessor_byte_stride(&accessor, &view);
                    if (outStride <= 0)
                    {
                        return nullptr;
                    }

                    const std::uint64_t offset = view.byte_offset + accessor.byte_offset;
                    if (offset >= buffer.data.count)
                    {
                        return nullptr;
                    }

                    return buffer.data.data + offset;
                }

                float readComponent(const uint8_t* inBytes, std::int32_t inType, std::int32_t inNormalized) const
                {
                    switch (inType)
                    {
                    case TG3_COMPONENT_TYPE_FLOAT:
                    {
                        float value = 0.0f;
                        std::memcpy(&value, inBytes, sizeof(float));

                        return value;
                    }
                    case TG3_COMPONENT_TYPE_DOUBLE:
                    {
                        double value = 0.0;
                        std::memcpy(&value, inBytes, sizeof(double));

                        return static_cast<float>(value);
                    }
                    case TG3_COMPONENT_TYPE_BYTE:
                    {
                        const std::int8_t value = static_cast<std::int8_t>(*inBytes);

                        return inNormalized ? (std::max)(static_cast<float>(value) / 127.0f, -1.0f)
                                            : static_cast<float>(value);
                    }
                    case TG3_COMPONENT_TYPE_UNSIGNED_BYTE:
                    {
                        return inNormalized ? static_cast<float>(*inBytes) / 255.0f : static_cast<float>(*inBytes);
                    }
                    case TG3_COMPONENT_TYPE_SHORT:
                    {
                        std::int16_t value = 0;
                        std::memcpy(&value, inBytes, sizeof(std::int16_t));

                        return inNormalized ? (std::max)(static_cast<float>(value) / 32767.0f, -1.0f)
                                            : static_cast<float>(value);
                    }
                    case TG3_COMPONENT_TYPE_UNSIGNED_SHORT:
                    {
                        std::uint16_t value = 0;
                        std::memcpy(&value, inBytes, sizeof(std::uint16_t));

                        return inNormalized ? static_cast<float>(value) / 65535.0f : static_cast<float>(value);
                    }
                    case TG3_COMPONENT_TYPE_INT:
                    {
                        std::int32_t value = 0;
                        std::memcpy(&value, inBytes, sizeof(std::int32_t));

                        return static_cast<float>(value);
                    }
                    case TG3_COMPONENT_TYPE_UNSIGNED_INT:
                    {
                        std::uint32_t value = 0;
                        std::memcpy(&value, inBytes, sizeof(std::uint32_t));

                        return static_cast<float>(value);
                    }
                    default:
                        return 0.0f;
                    }
                }

                bool readFloats(std::int32_t inAccessor, std::vector<float>& outValues, std::int32_t& outComponents) const
                {
                    const tg3_model& model = get();
                    if (inAccessor < 0 || static_cast<std::uint32_t>(inAccessor) >= model.accessors_count)
                    {
                        return false;
                    }

                    const tg3_accessor& accessor = model.accessors[inAccessor];
                    outComponents                = tg3_num_components(accessor.type);
                    if (outComponents <= 0)
                    {
                        return false;
                    }

                    std::int32_t   stride = 0;
                    const uint8_t* bytes  = accessorBytes(inAccessor, stride);
                    if (!bytes)
                    {
                        return false;
                    }

                    const std::int32_t componentSize = tg3_component_size(accessor.component_type);
                    if (componentSize <= 0)
                    {
                        return false;
                    }

                    outValues.resize(static_cast<std::size_t>(accessor.count) * static_cast<std::size_t>(outComponents));
                    for (std::uint64_t i = 0; i < accessor.count; ++i)
                    {
                        const uint8_t* element = bytes + (i * static_cast<std::uint64_t>(stride));
                        for (std::int32_t c = 0; c < outComponents; ++c)
                        {
                            outValues
                                [static_cast<std::size_t>(i) * static_cast<std::size_t>(outComponents) +
                                 static_cast<std::size_t>(c)] =
                                readComponent(
                                    element + (c * componentSize),
                                    accessor.component_type,
                                    accessor.normalized
                                );
                        }
                    }

                    return true;
                }

                bool readIndices(std::int32_t inAccessor, std::vector<std::uint32_t>& outIndices) const
                {
                    const tg3_model& model = get();
                    if (inAccessor < 0 || static_cast<std::uint32_t>(inAccessor) >= model.accessors_count)
                    {
                        return false;
                    }

                    const tg3_accessor& accessor = model.accessors[inAccessor];
                    std::int32_t        stride   = 0;
                    const uint8_t*      bytes    = accessorBytes(inAccessor, stride);
                    if (!bytes)
                    {
                        return false;
                    }

                    outIndices.resize(static_cast<std::size_t>(accessor.count));
                    for (std::uint64_t i = 0; i < accessor.count; ++i)
                    {
                        outIndices[static_cast<std::size_t>(i)] = static_cast<std::uint32_t>(
                            readComponent(bytes + (i * static_cast<std::uint64_t>(stride)), accessor.component_type, 0)
                        );
                    }

                    return true;
                }

                std::int32_t findAttribute(const tg3_primitive& inPrimitive, const char* inName) const
                {
                    for (std::uint32_t i = 0; i < inPrimitive.attributes_count; ++i)
                    {
                        if (equals(inPrimitive.attributes[i].key, inName))
                        {
                            return inPrimitive.attributes[i].value;
                        }
                    }

                    return -1;
                }

            private:
                void parse(const ModelRaw& inData, const String& inBaseDirectory, const String& inLabel)
                {
                    if (inData.empty())
                    {
                        throw std::runtime_error("The glTF [" + inLabel.toStandard() + "] data is empty");
                    }

                    tg3_parse_options options;
                    tg3_parse_options_init(&options);
                    options.images_as_is = 1;

                    const tg3_error_code error = tinygltf3::parse(
                        m_model,
                        m_errors,
                        inData.data(),
                        static_cast<std::uint64_t>(inData.size()),
                        inBaseDirectory.toChar(),
                        &options
                    );

                    if (error == TG3_OK && !m_errors.has_error())
                    {
                        return;
                    }

                    String message = "Failed to parse glTF [" + inLabel + "]";
                    if (m_errors.count() > 0)
                    {
                        const tg3_error_entry* entry = m_errors.entry(0);
                        if (entry && entry->message)
                        {
                            message.append(": ");
                            message.append(entry->message);
                        }
                    }

                    throw std::runtime_error(message.toStandard());
                }

            private:
                tinygltf3::Model      m_model;
                tinygltf3::ErrorStack m_errors;
            };

            std::int32_t findJointParent(
                std::int32_t                            inNode,
                const std::vector<std::int32_t>&        inParents,
                const std::unordered_set<std::int32_t>& inJoints
            )
            {
                std::int32_t parent = (inNode >= 0 && static_cast<std::size_t>(inNode) < inParents.size())
                                          ? inParents[static_cast<std::size_t>(inNode)]
                                          : -1;

                while (parent >= 0)
                {
                    if (inJoints.find(parent) != inJoints.end())
                    {
                        return parent;
                    }

                    parent = static_cast<std::size_t>(parent) < inParents.size()
                                 ? inParents[static_cast<std::size_t>(parent)]
                                 : -1;
                }

                return -1;
            }

            SkeletonBone buildBone(
                const Document&                                                    inDocument,
                std::int32_t                                                       inNode,
                const std::unordered_map<std::int32_t, std::vector<std::int32_t>>& inChildren
            )
            {
                SkeletonBone bone;
                bone.id        = inDocument.nodeName(inNode);
                bone.transform = convertTransform(inDocument.get().nodes[inNode]);

                const auto found = inChildren.find(inNode);
                if (found == inChildren.end())
                {
                    return bone;
                }

                for (const std::int32_t child : found->second)
                {
                    bone.children.push_back(buildBone(inDocument, child, inChildren));
                }

                return bone;
            }

            std::vector<std::int32_t> collectJoints(const Document& inDocument)
            {
                const tg3_model& model = inDocument.get();
                if (model.skins_count > 0 && model.skins[0].joints && model.skins[0].joints_count > 0)
                {
                    std::vector<std::int32_t> joints;
                    joints.reserve(model.skins[0].joints_count);

                    for (std::uint32_t i = 0; i < model.skins[0].joints_count; ++i)
                    {
                        const std::int32_t joint = model.skins[0].joints[i];
                        if (joint < 0 || static_cast<std::uint32_t>(joint) >= model.nodes_count)
                        {
                            continue;
                        }

                        joints.push_back(joint);
                    }

                    if (!joints.empty())
                    {
                        return joints;
                    }
                }

                std::vector<std::int32_t> nodes;
                nodes.reserve(model.nodes_count);
                for (std::uint32_t i = 0; i < model.nodes_count; ++i)
                {
                    nodes.push_back(static_cast<std::int32_t>(i));
                }

                return nodes;
            }

            const tg3_animation* findAnimation(const tg3_model& inModel, const String& inId)
            {
                if (inModel.animations_count == 0)
                {
                    return nullptr;
                }

                for (std::uint32_t i = 0; i < inModel.animations_count; ++i)
                {
                    if (toString(inModel.animations[i].name).equals(inId))
                    {
                        return &inModel.animations[i];
                    }
                }

                return &inModel.animations[0];
            }

            std::size_t findSample(const std::vector<float>& inTimes, float inTime)
            {
                if (inTimes.empty() || inTime <= inTimes.front())
                {
                    return 0;
                }

                for (std::size_t i = 0; i + 1 < inTimes.size(); ++i)
                {
                    if (inTime < inTimes[i + 1])
                    {
                        return i;
                    }
                }

                return inTimes.size() - 1;
            }

            float mixAmount(const std::vector<float>& inTimes, std::size_t inIndex, float inTime, bool inStep)
            {
                if (inStep || inIndex + 1 >= inTimes.size())
                {
                    return 0.0f;
                }

                const float span = inTimes[inIndex + 1] - inTimes[inIndex];
                if (std::fabs(span) <= 1e-8f)
                {
                    return 0.0f;
                }

                return std::clamp((inTime - inTimes[inIndex]) / span, 0.0f, 1.0f);
            }

            enum class ChannelPath : std::uint8_t
            {
                Translation,
                Rotation,
                Scale
            };

            struct SamplerCurve
            {
                std::vector<float>     times;
                std::vector<Vec3>      vectors;
                std::vector<QuatFloat> rotations;
                bool                   step = false;
            };

            struct NodeCurves
            {
                SamplerCurve translation;
                SamplerCurve rotation;
                SamplerCurve scale;
            };

            SamplerCurve readCurve(
                const Document&              inDocument,
                const tg3_animation_sampler& inSampler,
                ChannelPath                  inPath
            )
            {
                SamplerCurve curve;
                curve.step = equals(inSampler.interpolation, "STEP");

                std::vector<float> times;
                std::int32_t       timeComponents = 0;
                if (!inDocument.readFloats(inSampler.input, times, timeComponents) || timeComponents != 1 ||
                    times.empty())
                {
                    return curve;
                }

                std::vector<float> values;
                std::int32_t       valueComponents = 0;
                if (!inDocument.readFloats(inSampler.output, values, valueComponents))
                {
                    return curve;
                }

                const bool        cubic  = equals(inSampler.interpolation, "CUBICSPLINE");
                const std::size_t stride = cubic ? 3 : 1;
                if (values.size() < times.size() * stride * static_cast<std::size_t>(valueComponents))
                {
                    return curve;
                }

                curve.times = times;
                for (std::size_t i = 0; i < times.size(); ++i)
                {
                    const std::size_t valueIndex =
                        (i * stride + (cubic ? 1 : 0)) * static_cast<std::size_t>(valueComponents);
                    if (inPath == ChannelPath::Rotation)
                    {
                        const float x = valueIndex < values.size() ? values[valueIndex] : 0.0f;
                        const float y = valueIndex + 1 < values.size() ? values[valueIndex + 1] : 0.0f;
                        const float z = valueIndex + 2 < values.size() ? values[valueIndex + 2] : 0.0f;
                        const float w = valueIndex + 3 < values.size() ? values[valueIndex + 3] : 1.0f;
                        curve.rotations.push_back(convertRotation(x, y, z, w));

                        continue;
                    }

                    const float x = valueIndex < values.size() ? values[valueIndex] : 0.0f;
                    const float y = valueIndex + 1 < values.size() ? values[valueIndex + 1] : 0.0f;
                    const float z = valueIndex + 2 < values.size()
                                        ? values[valueIndex + 2]
                                        : (inPath == ChannelPath::Scale ? 1.0f : 0.0f);
                    if (inPath == ChannelPath::Scale)
                    {
                        curve.vectors.push_back(convertScale(x, y, z));

                        continue;
                    }

                    curve.vectors.push_back(convertVector(x, y, z));
                }

                return curve;
            }

            Vec3 sampleVector(const SamplerCurve& inCurve, const Vec3& inFallback, float inTime)
            {
                if (inCurve.times.empty() || inCurve.vectors.empty())
                {
                    return inFallback;
                }

                const std::size_t index  = findSample(inCurve.times, inTime);
                const float       amount = mixAmount(inCurve.times, index, inTime, inCurve.step);
                if (amount <= 0.0f || index + 1 >= inCurve.vectors.size())
                {
                    return inCurve.vectors[index];
                }

                const Vec3& start = inCurve.vectors[index];
                const Vec3& end   = inCurve.vectors[index + 1];

                return Vec3(
                    start.x + ((end.x - start.x) * amount),
                    start.y + ((end.y - start.y) * amount),
                    start.z + ((end.z - start.z) * amount)
                );
            }

            QuatFloat sampleRotation(const SamplerCurve& inCurve, const QuatFloat& inFallback, float inTime)
            {
                if (inCurve.times.empty() || inCurve.rotations.empty())
                {
                    return inFallback;
                }

                const std::size_t index  = findSample(inCurve.times, inTime);
                const float       amount = mixAmount(inCurve.times, index, inTime, inCurve.step);
                if (amount <= 0.0f || index + 1 >= inCurve.rotations.size())
                {
                    return inCurve.rotations[index];
                }

                const glm::quat mixed = glm::slerp(
                    static_cast<const glm::quat&>(inCurve.rotations[index]),
                    static_cast<const glm::quat&>(inCurve.rotations[index + 1]),
                    amount
                );

                return QuatFloat(mixed.w, mixed.x, mixed.y, mixed.z);
            }

            void collectTimes(const SamplerCurve& inCurve, std::vector<float>& outTimes)
            {
                outTimes.insert(outTimes.end(), inCurve.times.begin(), inCurve.times.end());
            }

            String uniqueName(const String& inName, std::unordered_set<String>& outUsed)
            {
                String name = inName.isEmpty() ? String("Mesh") : inName;
                if (outUsed.insert(name).second)
                {
                    return name;
                }

                std::uint32_t index = 1;
                while (true)
                {
                    const String candidate = String::sprint("%s_%u", name.toChar(), index);
                    if (outUsed.insert(candidate).second)
                    {
                        return candidate;
                    }

                    ++index;
                }
            }

            void appendTriangles(
                std::vector<std::uint32_t>&       outIndices,
                const std::vector<std::uint32_t>& inIndices,
                std::int32_t                      inMode
            )
            {
                const std::int32_t mode = inMode < 0 ? TG3_MODE_TRIANGLES : inMode;
                if (mode == TG3_MODE_TRIANGLES)
                {
                    outIndices.insert(outIndices.end(), inIndices.begin(), inIndices.end());

                    return;
                }

                if (mode == TG3_MODE_TRIANGLE_STRIP)
                {
                    for (std::size_t i = 2; i < inIndices.size(); ++i)
                    {
                        if ((i % 2) == 0)
                        {
                            outIndices.push_back(inIndices[i - 2]);
                            outIndices.push_back(inIndices[i - 1]);
                            outIndices.push_back(inIndices[i]);
                        }
                        else
                        {
                            outIndices.push_back(inIndices[i - 1]);
                            outIndices.push_back(inIndices[i - 2]);
                            outIndices.push_back(inIndices[i]);
                        }
                    }

                    return;
                }

                if (mode == TG3_MODE_TRIANGLE_FAN)
                {
                    for (std::size_t i = 2; i < inIndices.size(); ++i)
                    {
                        outIndices.push_back(inIndices[0]);
                        outIndices.push_back(inIndices[i - 1]);
                        outIndices.push_back(inIndices[i]);
                    }
                }
            }
        }
    }
}
