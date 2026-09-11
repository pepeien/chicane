#include "Chicane/Box/Mesh/Group.hpp"

#include <cmath>

namespace Chicane
{
    namespace Box
    {
        namespace
        {
            const AssetReference g_invalidTexture = {};

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

            void clearTransformAttributes(pugi::xml_node& outNode)
            {
                outNode.remove_attribute(MeshGroup::TRANSLATION_ATTRIBUTE_NAME);
                outNode.remove_attribute(MeshGroup::ROTATION_ATTRIBUTE_NAME);
                outNode.remove_attribute(MeshGroup::SCALE_ATTRIBUTE_NAME);
            }

            void clearTextureNodes(pugi::xml_node& outNode)
            {
                for (pugi::xml_node child = outNode.first_child(); child;)
                {
                    pugi::xml_node next = child.next_sibling();

                    const TextureMap map = toTextureMap(child.name());
                    if (map != TextureMap::Count)
                    {
                        outNode.remove_child(child);
                    }

                    child = next;
                }
            }
        }

        bool MeshGroup::isValid() const
        {
            return !m_id.isEmpty() && m_model.isValid() && hasTexture(TextureMap::Base) &&
                   getTexture(TextureMap::Base).isValid();
        }

        const String& MeshGroup::getId() const
        {
            return m_id;
        }

        void MeshGroup::setId(const String& inId)
        {
            m_id = inId;
        }

        const String& MeshGroup::getBone() const
        {
            return m_bone;
        }

        void MeshGroup::setBone(const String& inBone)
        {
            m_bone = inBone;
        }

        const AssetReference& MeshGroup::getModel() const
        {
            return m_model;
        }

        void MeshGroup::setModel(const pugi::xml_node& inNode, const FileSystem::Path& inBase)
        {
            m_model.setFrom(inNode, inBase);
        }

        void MeshGroup::setModel(const String& inSource, const String& inReference)
        {
            m_model.setSource(inSource);
            m_model.setReference(inReference);
        }

        const MeshGroup::TextureMaps& MeshGroup::getTextures() const
        {
            return m_textures;
        }

        bool MeshGroup::hasTexture(TextureMap inMap) const
        {
            return m_textures.find(inMap) != m_textures.end();
        }

        const AssetReference& MeshGroup::getTexture(TextureMap inMap) const
        {
            auto found = m_textures.find(inMap);
            if (found == m_textures.end())
            {
                return g_invalidTexture;
            }

            return found->second;
        }

        void MeshGroup::setTexture(TextureMap inMap, const pugi::xml_node& inNode, const FileSystem::Path& inBase)
        {
            AssetReference texture;
            texture.setFrom(inNode, inBase);
            m_textures[inMap] = texture;
        }

        void MeshGroup::setTexture(TextureMap inMap, const String& inSource, const String& inReference)
        {
            AssetReference texture;
            texture.setSource(inSource);
            texture.setReference(inReference);
            m_textures[inMap] = texture;
        }

        void MeshGroup::setTexture(const pugi::xml_node& inNode, const FileSystem::Path& inBase)
        {
            setTexture(TextureMap::Base, inNode, inBase);
        }

        void MeshGroup::setTexture(const String& inSource, const String& inReference)
        {
            setTexture(TextureMap::Base, inSource, inReference);
        }

        void MeshGroup::saveTextures(pugi::xml_node& outNode) const
        {
            if (Xml::isEmpty(outNode))
            {
                return;
            }

            clearTextureNodes(outNode);

            for (const auto& [map, texture] : m_textures)
            {
                if (!texture.isValid())
                {
                    continue;
                }

                pugi::xml_node textureNode = outNode.append_child(toString(map).toChar());
                texture.saveTo(textureNode);
            }
        }

        const Transform& MeshGroup::getTransform() const
        {
            return m_transform;
        }

        const Mat4& MeshGroup::getModelMatrix() const
        {
            return m_transform.getMatrix();
        }

        void MeshGroup::setTransform(const Transform& inTransform)
        {
            m_transform.setTransform(inTransform);
        }

        void MeshGroup::setTransform(const pugi::xml_node& inNode)
        {
            if (Xml::isEmpty(inNode))
            {
                m_transform = {};

                return;
            }

            Transform transform;
            transform.setTranslation(readVec3Attribute(inNode, TRANSLATION_ATTRIBUTE_NAME, Vec3::Zero()));
            transform.setRotation(readVec3Attribute(inNode, ROTATION_ATTRIBUTE_NAME, Vec3::Zero()));
            transform.setScale(readVec3Attribute(inNode, SCALE_ATTRIBUTE_NAME, Vec3::One()));

            m_transform.setTransform(transform);
        }

        void MeshGroup::saveTransform(pugi::xml_node& outNode) const
        {
            if (Xml::isEmpty(outNode))
            {
                return;
            }

            clearTransformAttributes(outNode);

            if (isIdentityTransform(m_transform))
            {
                return;
            }

            writeVec3Attribute(outNode, TRANSLATION_ATTRIBUTE_NAME, m_transform.getTranslation());
            writeVec3Attribute(outNode, ROTATION_ATTRIBUTE_NAME, m_transform.getRotation().getAngles());
            writeVec3Attribute(outNode, SCALE_ATTRIBUTE_NAME, m_transform.getScale());
        }
    }
}
