#include "Chicane/Box/Asset/Header.hpp"

#include "Chicane/Box/Asset.hpp"
#include "Chicane/Box/Asset/Font.hpp"
#include "Chicane/Box/Asset/Mesh.hpp"
#include "Chicane/Box/Asset/Model.hpp"
#include "Chicane/Box/Asset/Sky.hpp"
#include "Chicane/Box/Asset/Sound.hpp"
#include "Chicane/Box/Asset/Texture.hpp"

namespace Chicane
{
    namespace Box
    {
        const std::unordered_map<String, AssetType> TYPES = {
            { Sound::EXTENSION,   AssetType::Sound   },
            { Font::EXTENSION,    AssetType::Font    },
            { Mesh::EXTENSION,    AssetType::Mesh    },
            { Model::EXTENSION,   AssetType::Model   },
            { Sky::EXTENSION,     AssetType::Sky     },
            { Texture::EXTENSION, AssetType::Texture }
        };

        bool AssetHeader::isFileAsset(const FileSystem::Path& inFilepath)
        {
            if (inFilepath.empty() || !inFilepath.has_extension())
            {
                return false;
            }

            return TYPES.find(inFilepath.extension().string()) != TYPES.end();
        }

        AssetType AssetHeader::getType(const FileSystem::Path& inFilepath)
        {
            if (!isFileAsset(inFilepath))
            {
                return AssetType::Undefined;
            }

            return TYPES.at(inFilepath.extension().string());
        }

        AssetHeader::AssetHeader(const FileSystem::Path& inFilepath)
            : AssetHeader()
        {
            if (inFilepath.empty())
            {
                return;
            }

            const pugi::xml_document document = Xml::load(inFilepath);
            const pugi::xml_node root = document.first_child();

            filepath = inFilepath;
            fetchVersion(root);
            fetchId(root);
            fetchType();
        }

        AssetHeader::AssetHeader()
            : filepath(""),
            version(Asset::CURRENT_VERSION),
            id(""),
            type(AssetType::Undefined)
        {}

        void AssetHeader::fetchVersion(const pugi::xml_node& inRoot)
        {
            if (inRoot.empty())
            {
                version = Asset::CURRENT_VERSION;

                return;
            }

            version = Xml::getAttribute(Asset::VERSION_ATTRIBUTE_NAME, inRoot).as_uint();

            if (version > 0)
            {
                return;
            }

            version = Asset::CURRENT_VERSION;
        }

        void AssetHeader::fetchId(const pugi::xml_node& inRoot)
        {
            if (inRoot.empty())
            {
                id = "";

                return;
            }

            id = Xml::getAttribute(Asset::ID_ATTRIBUTE_NAME, inRoot).as_string();

            if (!id.isEmpty())
            {
                return;
            }

            id = "";
        }

        void AssetHeader::fetchType()
        {
            type = getType(filepath.string());
        }
    }
}