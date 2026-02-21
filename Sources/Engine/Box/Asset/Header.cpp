#include "Chicane/Box/Asset/Header.hpp"

#include "Chicane/Box/Asset.hpp"
#include "Chicane/Box/Font.hpp"
#include "Chicane/Box/Mesh.hpp"
#include "Chicane/Box/Model.hpp"
#include "Chicane/Box/Sky.hpp"
#include "Chicane/Box/Sound.hpp"
#include "Chicane/Box/Texture.hpp"

namespace Chicane
{
    namespace Box
    {
        const std::unordered_map<AssetType, String> TAGS = {
            {AssetType::Font,    Font::TAG   },
            {AssetType::Mesh,    Mesh::TAG   },
            {AssetType::Model,   Model::TAG  },
            {AssetType::Sky,     Sky::TAG    },
            {AssetType::Sound,   Sound::TAG  },
            {AssetType::Texture, Texture::TAG}
        };

        const std::unordered_map<AssetType, String> EXTENSIONS = {
            {AssetType::Font,    Font::EXTENSION   },
            {AssetType::Mesh,    Mesh::EXTENSION   },
            {AssetType::Model,   Model::EXTENSION  },
            {AssetType::Sky,     Sky::EXTENSION    },
            {AssetType::Sound,   Sound::EXTENSION  },
            {AssetType::Texture, Texture::EXTENSION}
        };

        bool AssetHeader::isFileAsset(const FileSystem::Path& inFilepath)
        {
            if (inFilepath.empty() || !inFilepath.has_extension())
            {
                return false;
            }

            const String value = inFilepath.extension().string();

            for (const auto& [type, extension] : EXTENSIONS)
            {
                if (!extension.equals(value))
                {
                    continue;
                }

                return true;
            }

            return false;
        }

        AssetType AssetHeader::getTypeFromExtension(const FileSystem::Path& inValue)
        {
            if (!isFileAsset(inValue))
            {
                return AssetType::Undefined;
            }

            const String value = inValue.extension().string();

            for (const auto& [type, extension] : EXTENSIONS)
            {
                if (!extension.equals(value))
                {
                    continue;
                }

                return type;
            }

            return AssetType::Undefined;
        }

        AssetType AssetHeader::getTypeFromTag(const String& inValue)
        {
            if (inValue.isEmpty())
            {
                return AssetType::Undefined;
            }

            const String value = inValue.trim();

            for (const auto& [type, tag] : TAGS)
            {
                if (!tag.equals(value))
                {
                    continue;
                }

                return type;
            }

            return AssetType::Undefined;
        }

        std::vector<String> AssetHeader::getTypeTags()
        {
            std::vector<String> result;

            for (const auto& [type, name] : TAGS)
            {
                result.push_back(name);
            }

            return result;
        }

        const String& AssetHeader::getTypeTag(AssetType inValue)
        {
            if (TAGS.find(inValue) == TAGS.end())
            {
                return String::empty();
            }

            return TAGS.at(inValue);
        }

        std::vector<String> AssetHeader::getTypeExtensions()
        {
            std::vector<String> result;

            for (const auto& [type, extension] : EXTENSIONS)
            {
                result.push_back(extension);
            }

            return result;
        }

        const String& AssetHeader::getTypeExtension(AssetType inValue)
        {
            if (EXTENSIONS.find(inValue) == EXTENSIONS.end())
            {
                return String::empty();
            }

            return EXTENSIONS.at(inValue);
        }

        AssetHeader::AssetHeader(const FileSystem::Path& inFilepath)
            : AssetHeader()
        {
            if (inFilepath.empty())
            {
                return;
            }

            const pugi::xml_document document = Xml::load(inFilepath);
            const pugi::xml_node     root     = document.first_child();

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
        }

        void AssetHeader::fetchType()
        {
            type = getTypeFromExtension(filepath);
        }
    }
}