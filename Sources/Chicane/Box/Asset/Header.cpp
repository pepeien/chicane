#include "Chicane/Box/Asset/Header.hpp"

#include "Chicane/Box/Asset.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Asset
        {
            const std::unordered_map<std::string, Type> TYPES = {
                { Audio::EXTENSION,   Type::Audio   },
                { Font::EXTENSION,    Type::Font    },
                { Mesh::EXTENSION,    Type::Mesh    },
                { Model::EXTENSION,   Type::Model   },
                { Sky::EXTENSION,     Type::Sky     },
                { Texture::EXTENSION, Type::Texture }
            };

            bool isFileAsset(const std::string& inFilepath)
            {
                if (inFilepath.empty())
                {
                    return false;
                }

                const std::string extension = String::split(
                    inFilepath,
                    "."
                ).back();

                return TYPES.find("." + extension) != TYPES.end();
            }

            Type getType(const std::string& inFilepath)
            {
                if (!isFileAsset(inFilepath))
                {
                    return Type::Undefined;
                }

                const std::string extension = String::split(
                    inFilepath,
                    "."
                ).back();

                return TYPES.at("." + extension);
            }

            Header Header::fromFilepath(const std::string& inFilepath)
            {
                if (inFilepath.empty())
                {
                    return {};
                }

                pugi::xml_document xml;

                XML::load(xml, inFilepath);

                return Header::fromXML(inFilepath, xml);
            }

            Header Header::fromXML(const std::string& inFilepath, const pugi::xml_document& inDocument)
            {
                const pugi::xml_node root = inDocument.first_child();

                Header header = {};
                header.filepath = inFilepath;
                header.fetchVersion(root);
                header.fetchId(root);
                header.fetchType();

                return header;
            }

            void Header::fetchVersion(const pugi::xml_node& inRoot)
            {
                if (inRoot.empty())
                {
                    version = CURRENT_VERSION;

                    return;
                }

                version = XML::getAttribute(VERSION_ATTRIBUTE_NAME, inRoot).as_uint();

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

                id = XML::getAttribute(ID_ATTRIBUTE_NAME, inRoot).as_string();

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