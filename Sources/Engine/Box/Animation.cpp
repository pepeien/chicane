#include "Chicane/Box/Animation.hpp"

#include <cmath>

#include "Chicane/Box/Skeleton.hpp"

#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Time.hpp"

namespace Chicane
{
    namespace Box
    {
        static bool isNearlyEqual(float inLeft, float inRight)
        {
            return std::fabs(inLeft - inRight) <= 1e-5f;
        }

        static bool isIdentityTransform(const Transform& inTransform)
        {
            const Vec3& translation = inTransform.getTranslation();
            const Vec3& rotation    = inTransform.getRotation().getAngles();
            const Vec3& scale       = inTransform.getScale();

            return isNearlyEqual(translation.x, 0.0f) && isNearlyEqual(translation.y, 0.0f) &&
                   isNearlyEqual(translation.z, 0.0f) && isNearlyEqual(rotation.x, 0.0f) &&
                   isNearlyEqual(rotation.y, 0.0f) && isNearlyEqual(rotation.z, 0.0f) && isNearlyEqual(scale.x, 1.0f) &&
                   isNearlyEqual(scale.y, 1.0f) && isNearlyEqual(scale.z, 1.0f);
        }

        static String toAttribute(const Vec3& inValue)
        {
            return String::sprint("%f,%f,%f", inValue.x, inValue.y, inValue.z);
        }

        static Vec3 readVec3Attribute(const XmlNode& inNode, const char* inName, const Vec3& inFallback)
        {
            return inNode.parseVec3(inName, inFallback);
        }

        static void writeVec3Attribute(XmlNode& outNode, const char* inName, const Vec3& inValue)
        {
            outNode.setAttribute(inName, toAttribute(inValue));
        }

        static AnimationLoop parseLoop(const String& inValue)
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

        static Transform readTransform(const XmlNode& inNode)
        {
            Transform transform;
            transform.setTranslation(readVec3Attribute(inNode, Animation::TRANSLATION_ATTRIBUTE_NAME, Vec3::Zero()));
            transform.setRotation(readVec3Attribute(inNode, Animation::ROTATION_ATTRIBUTE_NAME, Vec3::Zero()));
            transform.setScale(readVec3Attribute(inNode, Animation::SCALE_ATTRIBUTE_NAME, Vec3::One()));

            return transform;
        }

        static void writeTransform(XmlNode& outNode, const Transform& inTransform)
        {
            if (isIdentityTransform(inTransform))
            {
                return;
            }

            writeVec3Attribute(outNode, Animation::TRANSLATION_ATTRIBUTE_NAME, inTransform.getTranslation());
            writeVec3Attribute(outNode, Animation::ROTATION_ATTRIBUTE_NAME, inTransform.getRotation().getAngles());
            writeVec3Attribute(outNode, Animation::SCALE_ATTRIBUTE_NAME, inTransform.getScale());
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

            const XmlNode skeletonNode = getXML().getChild(Skeleton::TAG);
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

            const XmlNode clipNode = getXML().getChild(AnimationClip::TAG);
            if (Xml::isEmpty(clipNode))
            {
                return;
            }

            m_clip.duration =
                Time::fromSeconds(clipNode.parseFloat(AnimationClip::DURATION_ATTRIBUTE_NAME, 0.0f));
            m_clip.loop       = parseLoop(Xml::getAttribute(AnimationClip::LOOP_ATTRIBUTE_NAME, clipNode));
            m_clip.iterations = Xml::parseInt(
                Xml::getAttribute(AnimationClip::ITERATIONS_ATTRIBUTE_NAME, clipNode),
                1
            );

            for (const XmlNode& trackNode : clipNode.getChildren())
            {
                if (!String(trackNode.getName()).equals(AnimationTrack::TAG))
                {
                    continue;
                }

                AnimationTrack track;
                track.name = Xml::getAttribute(AnimationTrack::NAME_ATTRIBUTE_NAME, trackNode);

                for (const XmlNode& keyframeNode : trackNode.getChildren())
                {
                    if (!String(keyframeNode.getName()).equals(AnimationKeyframe::TAG))
                    {
                        continue;
                    }

                    AnimationKeyframe keyframe;
                    keyframe.time = Time::fromSeconds(
                        keyframeNode.parseFloat(AnimationKeyframe::TIME_ATTRIBUTE_NAME, 0.0f)
                    );
                    keyframe.easing =
                        Xml::getAttribute(AnimationKeyframe::EASING_ATTRIBUTE_NAME, keyframeNode);
                    keyframe.transform = readTransform(keyframeNode);
                    track.addKeyframe(keyframe);
                }

                m_clip.addTrack(track);
            }
        }

        void Animation::writeSkeleton()
        {
            XmlNode root         = getXML();
            XmlNode skeletonNode = root.getChild(Skeleton::TAG);
            if (Xml::isEmpty(skeletonNode))
            {
                skeletonNode = root.prependChild(Skeleton::TAG);
            }

            m_skeleton.saveTo(skeletonNode);
        }

        void Animation::writeClip()
        {
            XmlNode root     = getXML();
            XmlNode clipNode = root.getChild(AnimationClip::TAG);
            if (!Xml::isEmpty(clipNode))
            {
                root.removeChild(clipNode);
            }

            clipNode = root.appendChild(AnimationClip::TAG);
            Xml::addAttribute(
                clipNode,
                AnimationClip::DURATION_ATTRIBUTE_NAME,
                String::sprint("%f", m_clip.duration.seconds())
            );
            Xml::addAttribute(clipNode, AnimationClip::LOOP_ATTRIBUTE_NAME, toString(m_clip.loop));
            Xml::addAttribute(
                clipNode,
                AnimationClip::ITERATIONS_ATTRIBUTE_NAME,
                String::sprint("%d", m_clip.iterations)
            );

            for (const AnimationTrack& track : m_clip.tracks)
            {
                XmlNode trackNode = clipNode.appendChild(AnimationTrack::TAG);
                Xml::addAttribute(trackNode, AnimationTrack::NAME_ATTRIBUTE_NAME, track.name);

                for (const AnimationKeyframe& keyframe : track.keyframes)
                {
                    XmlNode keyframeNode = trackNode.appendChild(AnimationKeyframe::TAG);
                    Xml::addAttribute(
                        keyframeNode,
                        AnimationKeyframe::TIME_ATTRIBUTE_NAME,
                        String::sprint("%f", keyframe.time.seconds())
                    );

                    if (!keyframe.easing.isEmpty())
                    {
                        Xml::addAttribute(keyframeNode, AnimationKeyframe::EASING_ATTRIBUTE_NAME, keyframe.easing);
                    }

                    writeTransform(keyframeNode, keyframe.transform);
                }
            }
        }
    }
}
