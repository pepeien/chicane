#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Xml
    {
        bool isEmpty(const pugi::xml_document& inDocument)
        {
            return inDocument.children().empty();
        }

        bool isEmpty(const pugi::xml_node& inNode)
        {
            return inNode.children().empty() && !inNode.value();
        }

        void save(const pugi::xml_document& inDocument, const FileSystem::Path& inFilepath)
        {
            if (inFilepath.empty())
            {
                throw std::runtime_error("The XML document path is empty");
            }

            inDocument.save_file(
                inFilepath.c_str(),
                "    ",
                pugi::format_default | pugi::format_no_empty_element_tags | pugi::format_no_declaration
            );
        }

        pugi::xml_document load(const FileSystem::Path& inFilepath)
        {
            if (inFilepath.empty())
            {
                throw std::runtime_error("The XML document path is empty");
            }

            pugi::xml_document result;
            if (!result.load_file(inFilepath.c_str(), pugi::parse_default | pugi::parse_fragment))
            {
                throw std::runtime_error("Failed to read the XML document [" + inFilepath.string() + "]");
            }

            return result;
        }

        pugi::xml_attribute getAttribute(const String& inName, const pugi::xml_node& inNode)
        {
            return inNode.attribute(inName.toChar());
        }

        void addText(pugi::xml_node& outNode, const String& inText)
        {
            if (inText.isEmpty())
            {
                return;
            }

            outNode.text().set(inText.toStandard());
        }
    }
}