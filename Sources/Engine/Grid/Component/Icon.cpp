#include "Chicane/Grid/Component/Icon.reflected.hpp"

#include <cctype>
#include <stdexcept>

namespace Chicane
{
    namespace Grid
    {
        Icon::Icon(const pugi::xml_node& inNode)
            : Svg(inNode)
        {
            importStyleFile("Assets/Engine/UI/Components/Icon.decal");

            const String name = parseText(getAttribute(NAME_ATTRIBUTE_NAME)).trim();
            if (name.isEmpty())
            {
                return;
            }

            const String source = parseText(getAttribute(SOURCE_ATTRIBUTE_NAME)).trim();
            if (source.isEmpty())
            {
                throw std::runtime_error("Icon source folder is required");
            }

            applySource(resolveSource(name, source), inNode);
        }

        String Icon::toCamelCase(const String& inValue) const
        {
            String result;
            bool   bCapitalize = true;

            for (std::size_t i = 0; i < inValue.size(); i++)
            {
                const char character = inValue.at(i);
                if (character == '-' || character == '_' || character == ' ')
                {
                    bCapitalize = true;

                    continue;
                }

                if (bCapitalize)
                {
                    result += static_cast<char>(std::toupper(static_cast<unsigned char>(character)));
                    bCapitalize = false;

                    continue;
                }

                result += character;
            }

            return result;
        }

        FileSystem::Path Icon::resolveSource(const String& inName, const String& inFolder) const
        {
            String name = inName;

            if (name.contains("\\") || name.contains("..") || name.contains("/"))
            {
                throw std::runtime_error("Icon name must be a file stem [" + inName + "]");
            }

            if (name.endsWith(".grid") || name.endsWith(".svg"))
            {
                name = name.substr(0, name.lastOf('.'));
            }

            name = toCamelCase(name);

            FileSystem::Path folder(inFolder);
            if (folder.toString().contains(".."))
            {
                throw std::runtime_error("Icon source folder is invalid [" + inFolder + "]");
            }

            return folder / FileSystem::Path(name + ".grid");
        }

        void Icon::applySource(const FileSystem::Path& inPath, const pugi::xml_node& inUsage)
        {
            if (!inPath.exists())
            {
                throw std::runtime_error("Icon source does not exist [" + inPath.toString() + "]");
            }

            pugi::xml_document   document = Xml::load(inPath);
            const pugi::xml_node root     = document.first_child();
            if (root.empty())
            {
                throw std::runtime_error("Icon source does not have a root [" + inPath.toString() + "]");
            }

            m_sourceDocument.reset();
            m_sourceNode = m_sourceDocument.append_copy(root);

            for (pugi::xml_attribute attribute : inUsage.attributes())
            {
                const String key = attribute.name();
                if (key.equals(NAME_ATTRIBUTE_NAME, SOURCE_ATTRIBUTE_NAME))
                {
                    continue;
                }

                pugi::xml_attribute existing = m_sourceNode.attribute(attribute.name());
                if (!existing.empty())
                {
                    existing.set_value(attribute.value());

                    continue;
                }

                m_sourceNode.append_attribute(attribute.name()).set_value(attribute.value());
            }

            m_attributes = Xml::getAttributes(m_sourceNode);
            setId(getAttribute(ID_ATTRIBUTE_NAME));
            setClassName(getAttribute(CLASS_ATTRIBUTE_NAME));
        }
    }
}
