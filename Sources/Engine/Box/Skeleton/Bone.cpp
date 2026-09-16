#include "Chicane/Box/Skeleton/Bone.hpp"
#include "Chicane/Box/Skeleton/Bone/Entry.hpp"

#include <cmath>

#include "Chicane/Core/Math/Vec/Vec3.hpp"

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

        SkeletonBone::SkeletonBone()
            : id(""),
              transform({}),
              children({})
        {}

        SkeletonBoneEntry::SkeletonBoneEntry()
            : id(""),
              parentIndex(-1),
              transform({}),
              inverseBind(Mat4::One)
        {}

        void SkeletonBone::setFrom(const XmlNode& inNode)
        {
            id = Xml::getAttribute(ID_ATTRIBUTE_NAME, inNode);

            Transform rest;
            rest.setTranslation(readVec3Attribute(inNode, TRANSLATION_ATTRIBUTE_NAME, Vec3::Zero()));
            rest.setRotation(readVec3Attribute(inNode, ROTATION_ATTRIBUTE_NAME, Vec3::Zero()));
            rest.setScale(readVec3Attribute(inNode, SCALE_ATTRIBUTE_NAME, Vec3::One()));
            transform.setTransform(rest);

            children.clear();
            for (const XmlNode& childNode : inNode.getChildren())
            {
                if (!String(childNode.getName()).equals(TAG))
                {
                    continue;
                }

                SkeletonBone child;
                child.setFrom(childNode);
                children.push_back(child);
            }
        }

        void SkeletonBone::saveTo(XmlNode& outNode) const
        {
            Xml::addAttribute(outNode, ID_ATTRIBUTE_NAME, id);

            if (!isIdentityTransform(transform))
            {
                writeVec3Attribute(outNode, TRANSLATION_ATTRIBUTE_NAME, transform.getTranslation());
                writeVec3Attribute(outNode, ROTATION_ATTRIBUTE_NAME, transform.getRotation().getAngles());
                writeVec3Attribute(outNode, SCALE_ATTRIBUTE_NAME, transform.getScale());
            }

            for (const SkeletonBone& child : children)
            {
                XmlNode childNode = outNode.appendChild(TAG);
                child.saveTo(childNode);
            }
        }
    }
}
