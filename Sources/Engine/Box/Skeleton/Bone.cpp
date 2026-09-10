#include "Chicane/Box/Skeleton/Bone.hpp"

#include <cmath>

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
                pugi::xml_attribute attribute = outNode.attribute(inName);
                if (attribute.empty())
                {
                    Xml::addAttribute(outNode, inName, toAttribute(inValue));

                    return;
                }

                attribute.set_value(toAttribute(inValue).toStandard());
            }
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

        void SkeletonBone::setFrom(const pugi::xml_node& inNode)
        {
            id = Xml::getAttribute(ID_ATTRIBUTE_NAME, inNode).as_string();

            Transform rest;
            rest.setTranslation(readVec3Attribute(inNode, TRANSLATION_ATTRIBUTE_NAME, Vec3::Zero()));
            rest.setRotation(readVec3Attribute(inNode, ROTATION_ATTRIBUTE_NAME, Vec3::Zero()));
            rest.setScale(readVec3Attribute(inNode, SCALE_ATTRIBUTE_NAME, Vec3::One()));
            transform.setTransform(rest);

            children.clear();
            for (const pugi::xml_node& childNode : inNode.children())
            {
                if (!String(childNode.name()).equals(TAG))
                {
                    continue;
                }

                SkeletonBone child;
                child.setFrom(childNode);
                children.push_back(child);
            }
        }

        void SkeletonBone::saveTo(pugi::xml_node& outNode) const
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
                pugi::xml_node childNode = outNode.append_child(TAG);
                child.saveTo(childNode);
            }
        }
    }
}
