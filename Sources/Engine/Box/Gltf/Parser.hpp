#pragma once

#include "ChannelPath.hpp"
#include "Document.hpp"
#include "NodeCurves.hpp"
#include "SamplerCurve.hpp"
#include "Utility.hpp"

namespace Chicane
{
    namespace Box
    {
        inline std::int32_t findJointParent(
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

        inline SkeletonBone buildBone(
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

        inline std::vector<std::int32_t> collectJoints(const Document& inDocument)
        {
            const tg3_model&                 model = inDocument.get();
            std::vector<std::int32_t>        joints;
            std::unordered_set<std::int32_t> seen;

            const auto add = [&](std::int32_t inNode)
            {
                if (inNode < 0 || static_cast<std::uint32_t>(inNode) >= model.nodes_count)
                {
                    return;
                }

                if (seen.insert(inNode).second)
                {
                    joints.push_back(inNode);
                }
            };

            for (std::uint32_t s = 0; s < model.skins_count; ++s)
            {
                const tg3_skin& skin = model.skins[s];
                if (!skin.joints)
                {
                    continue;
                }

                for (std::uint32_t i = 0; i < skin.joints_count; ++i)
                {
                    add(skin.joints[i]);
                }
            }

            for (std::uint32_t a = 0; a < model.animations_count; ++a)
            {
                const tg3_animation& animation = model.animations[a];
                for (std::uint32_t c = 0; c < animation.channels_count; ++c)
                {
                    add(animation.channels[c].target.node);
                }
            }

            for (std::uint32_t i = 0; i < model.nodes_count; ++i)
            {
                if (model.nodes[i].mesh >= 0)
                {
                    add(static_cast<std::int32_t>(i));
                }
            }

            const std::vector<std::int32_t> parents = inDocument.parents();
            const std::vector<std::int32_t> seeds   = joints;
            for (const std::int32_t node : seeds)
            {
                std::int32_t parent = (node >= 0 && static_cast<std::size_t>(node) < parents.size())
                                          ? parents[static_cast<std::size_t>(node)]
                                          : -1;
                while (parent >= 0)
                {
                    add(parent);
                    parent = static_cast<std::size_t>(parent) < parents.size()
                                 ? parents[static_cast<std::size_t>(parent)]
                                 : -1;
                }
            }

            if (!joints.empty())
            {
                return joints;
            }

            joints.reserve(model.nodes_count);
            for (std::uint32_t i = 0; i < model.nodes_count; ++i)
            {
                joints.push_back(static_cast<std::int32_t>(i));
            }

            return joints;
        }

        inline const tg3_animation* findAnimation(const tg3_model& inModel, const String& inId)
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

        inline std::size_t findSample(const std::vector<float>& inTimes, float inTime)
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

        inline float mixAmount(const std::vector<float>& inTimes, std::size_t inIndex, float inTime, bool inStep)
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

        inline SamplerCurve readCurve(
            const Document& inDocument, const tg3_animation_sampler& inSampler, ChannelPath inPath
        )
        {
            SamplerCurve curve;
            curve.step = equals(inSampler.interpolation, "STEP");

            std::vector<float> times;
            std::int32_t       timeComponents = 0;
            if (!inDocument.readFloats(inSampler.input, times, timeComponents) || timeComponents != 1 || times.empty())
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
                const float z = valueIndex + 2 < values.size() ? values[valueIndex + 2]
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

        inline Vec3 sampleVector(const SamplerCurve& inCurve, const Vec3& inFallback, float inTime)
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

        inline QuatFloat sampleRotation(const SamplerCurve& inCurve, const QuatFloat& inFallback, float inTime)
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

        inline void collectTimes(const SamplerCurve& inCurve, std::vector<float>& outTimes)
        {
            outTimes.insert(outTimes.end(), inCurve.times.begin(), inCurve.times.end());
        }

        inline String uniqueName(const String& inName, std::unordered_set<String>& outUsed)
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

        inline void appendTriangles(
            std::vector<std::uint32_t>& outIndices, const std::vector<std::uint32_t>& inIndices, std::int32_t inMode
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
