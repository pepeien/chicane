#include "Chicane/Box/Animation.hpp"

#include <cmath>

#include "Chicane/Box/Skeleton.hpp"

#include "Chicane/Core/Math/Vec/Vec3.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace
        {
            bool isNearlyEqual(float inLeft, float inRight)
            {
                return std::fabs(inLeft - inRight) <= 1e-5f;
            }

            bool isIdentityTransform(const Transform& inTransform)
            {
                const Vec3& translation = inTransform.getTranslation();
                const Vec3& rotation    = inTransform.getRotation().getAngles();
                const Vec3& scale       = inTransform.getScale();

                return isNearlyEqual(translation.x, 0.0f) && isNearlyEqual(translation.y, 0.0f) &&
                       isNearlyEqual(translation.z, 0.0f) && isNearlyEqual(rotation.x, 0.0f) &&
                       isNearlyEqual(rotation.y, 0.0f) && isNearlyEqual(rotation.z, 0.0f) &&
                       isNearlyEqual(scale.x, 1.0f) && isNearlyEqual(scale.y, 1.0f) && isNearlyEqual(scale.z, 1.0f);
            }

            String toAttribute(const Vec3& inValue)
            {
                return String::sprint("%f,%f,%f", inValue.x, inValue.y, inValue.z);
            }

            Vec3 readVec3Attribute(const pugi::xml_node& inNode, const char* inName, const Vec3& inFallback)
            {
                const pugi::xml_attribute attribute = Xml::getAttribute(inName, inNode);
                if (attribute.empty())
                {
                    return inFallback;
                }

                const std::vector<String> values = String(attribute.as_string()).split(',');
                if (values.size() < 3)
                {
                    return inFallback;
                }

                return Vec3(
                    std::stof(values.at(0).toStandard()),
                    std::stof(values.at(1).toStandard()),
                    std::stof(values.at(2).toStandard())
                );
            }

            void writeVec3Attribute(pugi::xml_node& outNode, const char* inName, const Vec3& inValue)
            {
                Xml::addAttribute(outNode, inName, toAttribute(inValue));
            }

            AnimationLoop parseLoop(const String& inValue)
            {
                const String value = inValue.trim();
                if (value.equals("Repeat") || value.equals("Loop"))
                {
                    return AnimationLoop::Repeat;
                }

                if (value.equals("PingPong"))
                {
                    return AnimationLoop::PingPong;
                }

                return AnimationLoop::Once;
            }

            const char* loopToString(AnimationLoop inValue)
            {
                switch (inValue)
                {
                case AnimationLoop::Repeat:
                    return "Repeat";

                case AnimationLoop::PingPong:
                    return "PingPong";

                default:
                    return "Once";
                }
            }

            Transform readTransform(const pugi::xml_node& inNode)
            {
                Transform transform;
                transform.setTranslation(readVec3Attribute(inNode, Animation::TRANSLATION_ATTRIBUTE_NAME, Vec3::Zero()));
                transform.setRotation(readVec3Attribute(inNode, Animation::ROTATION_ATTRIBUTE_NAME, Vec3::Zero()));
                transform.setScale(readVec3Attribute(inNode, Animation::SCALE_ATTRIBUTE_NAME, Vec3::One()));

                return transform;
            }

            void writeTransform(pugi::xml_node& outNode, const Transform& inTransform)
            {
                if (isIdentityTransform(inTransform))
                {
                    return;
                }

                writeVec3Attribute(outNode, Animation::TRANSLATION_ATTRIBUTE_NAME, inTransform.getTranslation());
                writeVec3Attribute(outNode, Animation::ROTATION_ATTRIBUTE_NAME, inTransform.getRotation().getAngles());
                writeVec3Attribute(outNode, Animation::SCALE_ATTRIBUTE_NAME, inTransform.getScale());
            }
        }

        Animation::Animation(const FileSystem::Path& inFilepath)
            : Asset(inFilepath),
              m_skeleton({}),
              m_clip(getId())
        {
            fetchSkeleton();
            fetchClip();
        }

        bool Animation::hasSkeleton() const
        {
            return m_skeleton.isValid();
        }

        const AssetReference& Animation::getSkeleton() const
        {
            return m_skeleton;
        }

        void Animation::setSkeleton(const FileSystem::Path& inSource)
        {
            AssetReference reference;
            reference.setSource(inSource);
            setSkeleton(reference);
        }

        void Animation::setSkeleton(const AssetReference& inValue)
        {
            m_skeleton = inValue;
            writeSkeleton();
        }

        const AnimationClip& Animation::getClip() const
        {
            return m_clip;
        }

        void Animation::setClip(const AnimationClip& inClip)
        {
            m_clip = inClip;
            if (m_clip.name.isEmpty())
            {
                m_clip.name = getId();
            }

            writeClip();
        }

        void Animation::fetchSkeleton()
        {
            if (getFilepath().isEmpty() || isXMLEmpty())
            {
                return;
            }

            const pugi::xml_node skeletonNode = getXML().child(Skeleton::TAG);
            if (Xml::isEmpty(skeletonNode))
            {
                return;
            }

            m_skeleton.setFrom(skeletonNode, getFilepath());
        }

        void Animation::fetchClip()
        {
            m_clip      = AnimationClip(getId());
            m_clip.loop = AnimationLoop::Once;

            if (getFilepath().isEmpty() || isXMLEmpty())
            {
                return;
            }

            const pugi::xml_node clipNode = getXML().child(CLIP_TAG);
            if (Xml::isEmpty(clipNode))
            {
                return;
            }

            m_clip.duration   = Xml::getAttribute(CLIP_DURATION_ATTRIBUTE_NAME, clipNode).as_float();
            m_clip.loop       = parseLoop(Xml::getAttribute(CLIP_LOOP_ATTRIBUTE_NAME, clipNode).as_string());
            m_clip.iterations = Xml::getAttribute(CLIP_ITERATIONS_ATTRIBUTE_NAME, clipNode).as_int(1);

            for (const pugi::xml_node& trackNode : clipNode.children())
            {
                if (!String(trackNode.name()).equals(TRACK_TAG))
                {
                    continue;
                }

                AnimationTrack track;
                track.name = Xml::getAttribute(TRACK_NAME_ATTRIBUTE_NAME, trackNode).as_string();

                for (const pugi::xml_node& keyframeNode : trackNode.children())
                {
                    if (!String(keyframeNode.name()).equals(KEYFRAME_TAG))
                    {
                        continue;
                    }

                    AnimationKeyframe keyframe;
                    keyframe.time      = Xml::getAttribute(KEYFRAME_TIME_ATTRIBUTE_NAME, keyframeNode).as_float();
                    keyframe.easing    = Xml::getAttribute(KEYFRAME_EASING_ATTRIBUTE_NAME, keyframeNode).as_string();
                    keyframe.transform = readTransform(keyframeNode);
                    track.addKeyframe(keyframe);
                }

                m_clip.addTrack(track);
            }
        }

        void Animation::writeSkeleton()
        {
            pugi::xml_node root         = getXML();
            pugi::xml_node skeletonNode = root.child(Skeleton::TAG);
            if (Xml::isEmpty(skeletonNode))
            {
                skeletonNode = root.prepend_child(Skeleton::TAG);
            }

            m_skeleton.saveTo(skeletonNode);
        }

        void Animation::writeClip()
        {
            pugi::xml_node root     = getXML();
            pugi::xml_node clipNode = root.child(CLIP_TAG);
            if (!Xml::isEmpty(clipNode))
            {
                root.remove_child(clipNode);
            }

            clipNode = root.append_child(CLIP_TAG);
            Xml::addAttribute(clipNode, CLIP_DURATION_ATTRIBUTE_NAME, String::sprint("%f", m_clip.duration));
            Xml::addAttribute(clipNode, CLIP_LOOP_ATTRIBUTE_NAME, loopToString(m_clip.loop));
            Xml::addAttribute(clipNode, CLIP_ITERATIONS_ATTRIBUTE_NAME, String::sprint("%d", m_clip.iterations));

            for (const AnimationTrack& track : m_clip.tracks)
            {
                pugi::xml_node trackNode = clipNode.append_child(TRACK_TAG);
                Xml::addAttribute(trackNode, TRACK_NAME_ATTRIBUTE_NAME, track.name);

                for (const AnimationKeyframe& keyframe : track.keyframes)
                {
                    pugi::xml_node keyframeNode = trackNode.append_child(KEYFRAME_TAG);
                    Xml::addAttribute(keyframeNode, KEYFRAME_TIME_ATTRIBUTE_NAME, String::sprint("%f", keyframe.time));

                    if (!keyframe.easing.isEmpty())
                    {
                        Xml::addAttribute(keyframeNode, KEYFRAME_EASING_ATTRIBUTE_NAME, keyframe.easing);
                    }

                    writeTransform(keyframeNode, keyframe.transform);
                }
            }
        }
    }
}
