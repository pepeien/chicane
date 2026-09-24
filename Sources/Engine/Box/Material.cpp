#include "Chicane/Box/Material.hpp"

#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        const AssetReference g_invalidMaterialTexture = {};

        static void clearTextureNodes(XmlNode& outNode)
        {
            for (XmlNode child = outNode.getFirstChild(); child;)
            {
                XmlNode next = child.getNextSibling();

                const TextureMaterial type = toTextureMaterial(child.getName());
                if (type != TextureMaterial::Count)
                {
                    outNode.removeChild(child);
                }

                child = next;
            }
        }

        const Material* Material::getDefault()
        {
            return Box::load<Material>(DEFAULT_SOURCE);
        }

        Material::Material(const FileSystem::Path& inFilepath)
            : Asset(inFilepath)
        {
            fetchTextures();
        }

        bool Material::isValid() const
        {
            return hasTexture(TextureMaterial::Albedo) && getTexture(TextureMaterial::Albedo).isValid();
        }

        const Material::TextureMaps& Material::getTextures() const
        {
            return m_textures;
        }

        bool Material::hasTexture(TextureMaterial inType) const
        {
            return m_textures.find(inType) != m_textures.end();
        }

        const AssetReference& Material::getTexture(TextureMaterial inType) const
        {
            auto found = m_textures.find(inType);
            if (found == m_textures.end())
            {
                return g_invalidMaterialTexture;
            }

            return found->second;
        }

        void Material::setTexture(TextureMaterial inType, const XmlNode& inNode, const FileSystem::Path& inBase)
        {
            AssetReference texture;
            texture.setFrom(inNode, inBase);

            m_textures[inType] = texture;
            saveTextures();
        }

        void Material::setTexture(TextureMaterial inType, const String& inSource, const String& inReference)
        {
            AssetReference texture;
            texture.setSource(inSource);
            texture.setReference(inReference);

            m_textures[inType] = texture;
            saveTextures();
        }

        void Material::setTexture(const String& inSource, const String& inReference)
        {
            setTexture(TextureMaterial::Albedo, inSource, inReference);
        }

        void Material::fetchTextures()
        {
            m_textures.clear();

            if (getFilepath().isEmpty() || isXMLEmpty())
            {
                return;
            }

            for (const XmlNode& child : getXML().getChildren())
            {
                const TextureMaterial type = toTextureMaterial(child.getName());
                if (type == TextureMaterial::Count)
                {
                    continue;
                }

                AssetReference texture;
                texture.setFrom(child, getFilepath());
                m_textures[type] = texture;
            }
        }

        void Material::saveTextures()
        {
            XmlNode root = getXML();
            if (Xml::isEmpty(root))
            {
                return;
            }

            clearTextureNodes(root);

            for (const auto& [type, texture] : m_textures)
            {
                if (!texture.isValid())
                {
                    continue;
                }

                XmlNode textureNode = root.appendChild(toString(type).toChar());
                texture.saveTo(textureNode);
            }
        }
    }
}
