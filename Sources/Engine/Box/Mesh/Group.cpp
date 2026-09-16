#include "Chicane/Box/Mesh/Group.hpp"

#include <algorithm>
#include <cmath>

namespace Chicane
{
    namespace Box
    {
        const AssetReference g_invalidTexture = {};

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

        static void clearTransformAttributes(XmlNode& outNode)
        {
            outNode.removeAttribute(MeshGroup::TRANSLATION_ATTRIBUTE_NAME);
            outNode.removeAttribute(MeshGroup::ROTATION_ATTRIBUTE_NAME);
            outNode.removeAttribute(MeshGroup::SCALE_ATTRIBUTE_NAME);
        }

        static void clearTextureNodes(XmlNode& outNode)
        {
            for (XmlNode child = outNode.getFirstChild(); child;)
            {
                XmlNode next = child.getNextSibling();

                const TextureMap map = toTextureMap(child.getName());
                if (map != TextureMap::Count)
                {
                    outNode.removeChild(child);
                }

                child = next;
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

        void MeshGroup::setModel(const XmlNode& inNode, const FileSystem::Path& inBase)
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

        void MeshGroup::setTexture(TextureMap inMap, const XmlNode& inNode, const FileSystem::Path& inBase)
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

        void MeshGroup::setTexture(const XmlNode& inNode, const FileSystem::Path& inBase)
        {
            setTexture(TextureMap::Base, inNode, inBase);
        }

        void MeshGroup::setTexture(const String& inSource, const String& inReference)
        {
            setTexture(TextureMap::Base, inSource, inReference);
        }

        void MeshGroup::saveTextures(XmlNode& outNode) const
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

                XmlNode textureNode = outNode.appendChild(toString(map).toChar());
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

        void MeshGroup::setTransform(const XmlNode& inNode)
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

        void MeshGroup::saveTransform(XmlNode& outNode) const
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

        float MeshGroup::getEmissiveStrength() const
        {
            return m_emissiveStrength;
        }

        void MeshGroup::setEmissiveStrength(float inValue)
        {
            m_emissiveStrength = std::max(0.0f, inValue);
        }

        float MeshGroup::getTileSize() const
        {
            return m_tileSize;
        }

        void MeshGroup::setTileSize(float inValue)
        {
            m_tileSize = std::max(0.0f, inValue);
        }
    }
}
