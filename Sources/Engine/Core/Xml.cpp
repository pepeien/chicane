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
            return inNode.empty();
        }

        void save(const pugi::xml_document& inDocument, const FileSystem::Path& inFilepath)
        {
            if (inFilepath.empty())
            {
                throw std::runtime_error("The XML document path is empty");
            }

            if (!inDocument.save_file(
                    inFilepath.c_str(),
                    "    ",
                    pugi::format_default | pugi::format_no_empty_element_tags | pugi::format_no_declaration
                ))
            {
                throw std::runtime_error("Failed to save the XML [ " + inFilepath.string() + " ]");
            }
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

        Attributes getAttributes(const pugi::xml_node& inNode)
        {
            Attributes result;

            for (pugi::xml_attribute attribute : inNode.attributes())
            {
                result.insert(std::make_pair(attribute.name(), attribute.as_string()));
            }

            return result;
        }

        pugi::xml_attribute getAttribute(const String& inName, const pugi::xml_node& inNode)
        {
            return inNode.attribute(inName.toChar());
        }

        void addAttribute(pugi::xml_node& outNode, const String& inName, const String& inValue)
        {
            if (inName.isEmpty())
            {
                return;
            }

            outNode.append_attribute(inName.toChar()).set_value(inValue);
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