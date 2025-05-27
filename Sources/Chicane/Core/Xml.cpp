#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace XML
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
                throw std::runtime_error("The document filepath is invalid");
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
                throw std::runtime_error("The document filepath is invalid");
            }

            pugi::xml_document result;
            if (!result.load_file(inFilepath.c_str(), pugi::parse_default | pugi::parse_fragment))
            {
                throw std::runtime_error("Failed to read " + inFilepath.string());
            }

            return result;
        }

        pugi::xml_attribute getAttribute(const std::string& inName, const pugi::xml_node& inNode)
        {
            return inNode.attribute(inName.c_str());
        }

        void addText(pugi::xml_node& outNode, const std::string& inText)
        {
            if (inText.empty())
            {
                return;
            }

            outNode.text().set(inText);
        }
    }
}