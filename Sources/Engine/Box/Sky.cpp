#include "Chicane/Box/Sky.hpp"

#include <algorithm>

#include "Chicane/Box/Model.hpp"
#include "Chicane/Box/Texture.hpp"

#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        Sky::Sky(const FileSystem::Path& inFilepath)
            : Asset(inFilepath),
              m_kind(SkyKind::Cube),
              m_textures({}),
              m_model()
        {
            fetchTextures();
            fetchKind();
            fetchModel();
        }

        SkyKind Sky::getKind() const
        {
            return m_kind;
        }

        void Sky::setKind(SkyKind inKind)
        {
            m_kind = inKind;

            setAttribute(KIND_ATTRIBUTE_NAME, toString(m_kind));

            XmlNode leftover = getXML().getChild("Kind");
            if (!leftover.empty())
            {
                getXML().removeChild(leftover);
            }
        }

        const AssetReference::List& Sky::getTextures() const
        {
            return m_textures;
        }

        void Sky::addTexture(const std::vector<FileSystem::Path>& inFilepaths)
        {
            for (const FileSystem::Path& path : inFilepaths)
            {
                addTexture(path);
            }
        }

        void Sky::addTexture(const FileSystem::Path& inFilepath)
        {
            if (!FileSystem::exists(inFilepath))
            {
                throw std::runtime_error("Texture reference file [" + inFilepath.toString() + "] was not found");
            }

            Box::Texture asset(inFilepath);

            addTexture(asset.getFilepath(), asset.getId());
        }

        void Sky::addTexture(const String& inSource, const String& inReference)
        {
            XmlNode textures = getXML().getChild(TEXTURES_TAG);

            if (textures.empty())
            {
                textures = getXML().appendChild(TEXTURES_TAG);
            }

            XmlNode node = textures.appendChild(Texture::TAG);

            AssetReference texture;
            texture.setSource(inSource);
            texture.setReference(inReference);
            texture.saveTo(node);

            m_textures.push_back(texture);
        }

        const AssetReference& Sky::getModel() const
        {
            return m_model;
        }

        void Sky::setModel(const FileSystem::Path& inFilepath)
        {
            if (!FileSystem::exists(inFilepath))
            {
                throw std::runtime_error("Model reference file [" + inFilepath.toString() + "] was not found");
            }

            Box::Model asset(inFilepath);

            setModel(asset.getFilepath(), asset.getId());
        }

        void Sky::setModel(const String& inSource, const String& inReference)
        {
            XmlNode model = getXML().getChild(Model::TAG);

            if (model.empty())
            {
                model = getXML().appendChild(Model::TAG);
            }

            m_model.setSource(inSource);
            m_model.setReference(inReference);
            m_model.saveTo(model);
        }

        void Sky::fetchKind()
        {
            String value = String(getAttribute(KIND_ATTRIBUTE_NAME)).trim();
            if (value.isEmpty())
            {
                const XmlNode node = getXML().getChild("Kind");
                if (!node.empty())
                {
                    value = String(node.getText()).trim();
                }
            }

            if (value.equals("Panorama"))
            {
                m_kind = SkyKind::Panorama;

                return;
            }

            if (!value.isEmpty())
            {
                m_kind = SkyKind::Cube;

                return;
            }

            if (m_textures.size() == 1)
            {
                m_kind = SkyKind::Panorama;
            }
        }

        void Sky::fetchTextures()
        {
            const XmlNode textures = getXML().getChild(TEXTURES_TAG);

            if (textures.empty())
            {
                return;
            }

            for (const XmlNode& node : textures.getChildren(Texture::TAG))
            {
                AssetReference texture;
                texture.setFrom(node, getFilepath());

                m_textures.push_back(texture);
            }
        }

        void Sky::fetchModel()
        {
            const XmlNode model = getXML().getChild(Model::TAG);
            if (!model.empty())
            {
                m_model.setFrom(model, getFilepath());
            }

            if (!m_model.getSource().isEmpty())
            {
                return;
            }

            m_model.setSource(m_kind == SkyKind::Panorama ? DOME_SOURCE : BOX_SOURCE);
            m_model.setReference(Model::DEFAULT_REFERENCE);
        }
    }
}