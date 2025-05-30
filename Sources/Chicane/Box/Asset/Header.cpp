#include "Chicane/Box/Asset/Header.hpp"

#include "Chicane/Box/Asset.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Asset
        {
            const std::unordered_map<std::string, Type> TYPES = {
                { Sound::EXTENSION,   Type::Sound   },
                { Font::EXTENSION,    Type::Font    },
                { Mesh::EXTENSION,    Type::Mesh    },
                { Model::EXTENSION,   Type::Model   },
                { Sky::EXTENSION,     Type::Sky     },
                { Texture::EXTENSION, Type::Texture }
            };

            bool isFileAsset(const FileSystem::Path& inFilepath)
            {
                if (inFilepath.empty() || !inFilepath.has_extension())
                {
                    return false;
                }

                return TYPES.find(inFilepath.extension().string()) != TYPES.end();
            }

            Type getType(const FileSystem::Path& inFilepath)
            {
                if (!isFileAsset(inFilepath))
                {
                    return Type::Undefined;
                }

                return TYPES.at(inFilepath.extension().string());
            }

            Header::Header(const FileSystem::Path& inFilepath)
                : Header()
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

            Header::Header()
                : filepath(""),
                version(CURRENT_VERSION),
                id(""),
                type(Type::Undefined)
            {}

            void Header::fetchVersion(const pugi::xml_node& inRoot)
            {
                if (inRoot.empty())
                {
                    version = CURRENT_VERSION;

                    return;
                }

                version = Xml::getAttribute(VERSION_ATTRIBUTE_NAME, inRoot).as_uint();

                if (version > 0)
                {
                    return;
                }

                version = CURRENT_VERSION;
            }

            void Header::fetchId(const pugi::xml_node& inRoot)
            {
                if (inRoot.empty())
                {
                    id = "";

                    return;
                }

                id = Xml::getAttribute(ID_ATTRIBUTE_NAME, inRoot).as_string();

                if (!id.empty())
                {
                    return;
                }

                id = "";
            }

            void Header::fetchType()
            {
                type = getType(filepath.string());
            }
        }
    }
}