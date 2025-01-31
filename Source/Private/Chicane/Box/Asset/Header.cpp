#include "Chicane/Box/Asset/Header.hpp"

#include "Chicane/Box/Asset.hpp"
#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Asset
        {
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
                if (filepath.empty())
                {
                    type = Type::Undefined;

                    return;
                }

                const std::string extension = Utils::trim(filepath.extension().string());

                if (TYPES.find(extension) == TYPES.end())
                {
                    type = Type::Undefined;

                    return;
                }

                type = TYPES.at(extension);
            }
        }
    }
}