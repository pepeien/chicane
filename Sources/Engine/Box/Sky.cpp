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
            : Asset(inFilepath)
        {
            fetchTextures();
            fetchModel();
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
                throw std::runtime_error("Texture reference file [" + inFilepath.string() + "] was not found");
            }

            Box::Texture asset(inFilepath);

            addTexture(asset.getFilepath().string(), asset.getId());
        }

        void Sky::addTexture(const String& inSource, const String& inReference)
        {
            pugi::xml_node textures = getXML().child(TEXTURES_TAG);

            if (textures.empty())
            {
                textures = getXML().append_child(TEXTURES_TAG);
            }

            pugi::xml_node node = textures.append_child(Texture::TAG);

            AssetReference texture;
            texture.setSource(inSource.toStandard());
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
                throw std::runtime_error("Model reference file [" + inFilepath.string() + "] was not found");
            }

            Box::Model asset(inFilepath);

            setModel(asset.getFilepath().string(), asset.getId());
        }

        void Sky::setModel(const String& inSource, const String& inReference)
        {
            pugi::xml_node model = getXML().child(Model::TAG);

            if (model.empty())
            {
                model = getXML().append_child(Model::TAG);
            }

            m_model.setSource(inSource.toStandard());
            m_model.setReference(inReference);
            m_model.saveTo(model);
        }

        void Sky::fetchTextures()
        {
            const pugi::xml_node textures = getXML().child(TEXTURES_TAG);

            if (textures.empty())
            {
                return;
            }

            for (const pugi::xml_node& node : textures.children(Texture::TAG))
            {
                AssetReference texture;
                texture.setFrom(node);

                m_textures.push_back(texture);
            }
        }

        void Sky::fetchModel()
        {
            const pugi::xml_node model = getXML().child(Model::TAG);

            if (model.empty())
            {
                return;
            }

            m_model.setFrom(model);
        }
    }
}