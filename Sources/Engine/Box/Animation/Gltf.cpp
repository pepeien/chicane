#include "Chicane/Box/Animation/Gltf.hpp"

#include "../Gltf/Parser.hpp"

#include "Chicane/Box/Animation/Keyframe.hpp"
#include "Chicane/Box/Animation/Loop.hpp"
#include "Chicane/Box/Animation/Track.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace AnimationGltf
        {
            AnimationClip parse(const FileSystem::Path& inFilepath, const String& inId)
            {
                const Document       document(inFilepath);
                const tg3_model&     model     = document.get();
                const tg3_animation* animation = findAnimation(model, inId);
                if (!animation)
                {
                    throw std::runtime_error(
                        "The glTF file [" + inFilepath.toString().toStandard() + "] has no animations"
                    );
                }

                std::unordered_map<std::int32_t, NodeCurves> curves;
                for (std::uint32_t i = 0; i < animation->channels_count; ++i)
                {
                    const tg3_animation_channel& channel = animation->channels[i];
                    if (channel.target.node < 0 ||
                        static_cast<std::uint32_t>(channel.target.node) >= model.nodes_count || channel.sampler < 0 ||
                        static_cast<std::uint32_t>(channel.sampler) >= animation->samplers_count)
                    {
                        continue;
                    }

                    if (equals(channel.target.path, "weights"))
                    {
                        continue;
                    }

                    const tg3_animation_sampler& sampler = animation->samplers[channel.sampler];
                    NodeCurves&                  node    = curves[channel.target.node];
                    if (equals(channel.target.path, "translation"))
                    {
                        node.translation = readCurve(document, sampler, ChannelPath::Translation);

                        continue;
                    }

                    if (equals(channel.target.path, "rotation"))
                    {
                        node.rotation = readCurve(document, sampler, ChannelPath::Rotation);

                        continue;
                    }

                    if (equals(channel.target.path, "scale"))
                    {
                        node.scale = readCurve(document, sampler, ChannelPath::Scale);
                    }
                }

                if (curves.empty())
                {
                    throw std::runtime_error(
                        "The glTF animation [" + toString(animation->name).toStandard() + "] has no bone tracks"
                    );
                }

                AnimationClip clip(inId.isEmpty() ? toString(animation->name) : inId);
                clip.loop       = AnimationLoop::Once;
                clip.iterations = 1;

                for (const auto& [nodeIndex, nodeCurves] : curves)
                {
                    const Transform    rest = convertTransform(model.nodes[nodeIndex]);
                    std::vector<float> times;
                    collectTimes(nodeCurves.translation, times);
                    collectTimes(nodeCurves.rotation, times);
                    collectTimes(nodeCurves.scale, times);
                    if (times.empty())
                    {
                        continue;
                    }

                    std::sort(times.begin(), times.end());
                    times.erase(std::unique(times.begin(), times.end()), times.end());

                    AnimationTrack track(document.nodeName(nodeIndex));
                    const bool     step = (nodeCurves.translation.times.empty() || nodeCurves.translation.step) &&
                                      (nodeCurves.rotation.times.empty() || nodeCurves.rotation.step) &&
                                      (nodeCurves.scale.times.empty() || nodeCurves.scale.step);

                    for (const float time : times)
                    {
                        AnimationKeyframe keyframe;
                        keyframe.time   = time;
                        keyframe.easing = step ? "Step" : "";
                        keyframe.transform.setTranslation(
                            sampleVector(nodeCurves.translation, rest.getTranslation(), time)
                        );
                        keyframe.transform.setRotation(
                            sampleRotation(nodeCurves.rotation, rest.getRotation().get(), time)
                        );
                        keyframe.transform.setScale(sampleVector(nodeCurves.scale, rest.getScale(), time));
                        track.addKeyframe(keyframe);

                        clip.duration = (std::max)(clip.duration, time);
                    }

                    clip.addTrack(track);
                }

                if (clip.tracks.empty())
                {
                    throw std::runtime_error(
                        "The glTF animation [" + toString(animation->name).toStandard() + "] has no bone tracks"
                    );
                }

                return clip;
            }
        }
    }
}
