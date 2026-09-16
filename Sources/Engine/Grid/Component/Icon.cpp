#include "Chicane/Grid/Component/Icon.reflected.hpp"

#include <cctype>
#include <memory>
#include <stdexcept>
#include <unordered_map>

namespace Chicane
{
    namespace Grid
    {
        static XmlNode findSource(const FileSystem::Path& inPath)
        {
            static std::unordered_map<std::string, std::unique_ptr<XmlDocument>> cache;

            const std::string key = inPath.toString().toStandard();

            auto found = cache.find(key);

            if (found == cache.end())
            {
                if (!inPath.exists())
                {
                    throw std::runtime_error("Icon source does not exist [" + inPath.toString() + "]");
                }

                std::unique_ptr<XmlDocument> document = std::make_unique<XmlDocument>();
                *document                                    = Xml::load(inPath);

                found = cache.emplace(key, std::move(document)).first;
            }

            return found->second->getFirstChild();
        }

        Icon::Icon(const XmlNode& inNode)
            : Svg(inNode),
              m_nameBinding(getAttribute(NAME_ATTRIBUTE_NAME)),
              m_sourceBinding(getAttribute(SOURCE_ATTRIBUTE_NAME)),
              m_resolvedName(String::empty()),
              m_usageAttributes(m_attributes)
        {
            importStyleFile("Assets/Engine/UI/Components/Icon.decal");

            refreshSource();
        }

        void Icon::onRefresh()
        {
            refreshSource();

            Svg::onRefresh();
        }

        void Icon::onTick(float inDeltaTime)
        {
            if (isReference(m_nameBinding) || isReference(m_sourceBinding))
            {
                refreshSource();
            }

            Svg::onTick(inDeltaTime);
        }

        void Icon::refreshSource()
        {
            const String name = parseText(m_nameBinding).trim();
            if (name.isEmpty() || name.equals(m_resolvedName))
            {
                return;
            }

            const bool bIsBound = isReference(m_nameBinding) || isReference(m_sourceBinding);

            const String source = parseText(m_sourceBinding).trim();
            if (source.isEmpty())
            {
                if (bIsBound)
                {
                    return;
                }

                throw std::runtime_error("Icon source folder is required");
            }

            FileSystem::Path path;
            try
            {
                path = resolveSource(name, source);
            }
            catch (const std::runtime_error&)
            {
                if (bIsBound)
                {
                    return;
                }

                throw;
            }

            if (!path.exists())
            {
                if (bIsBound)
                {
                    return;
                }

                throw std::runtime_error("Icon source does not exist [" + path.toString() + "]");
            }

            const bool bDidLoad = !m_resolvedName.isEmpty();

            m_resolvedName = name;

            applySource(path);

            if (bDidLoad)
            {
                markStyleDirtySubtree();
                markLayoutDirty();
            }
        }

        String Icon::toCamelCase(const String& inValue) const
        {
            String result;
            bool   bShouldCapitalize = true;

            for (std::size_t i = 0; i < inValue.size(); i++)
            {
                const char character = inValue.at(i);
                if (character == '-' || character == '_' || character == ' ')
                {
                    bShouldCapitalize = true;

                    continue;
                }

                if (bShouldCapitalize)
                {
                    result += static_cast<char>(std::toupper(static_cast<unsigned char>(character)));
                    bShouldCapitalize = false;

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

        void Icon::applySource(const FileSystem::Path& inPath)
        {
            const XmlNode root = findSource(inPath);
            if (root.empty())
            {
                throw std::runtime_error("Icon source does not have a root [" + inPath.toString() + "]");
            }

            parse(root);

            for (const auto& [key, value] : m_usageAttributes)
            {
                if (key.equals(NAME_ATTRIBUTE_NAME, SOURCE_ATTRIBUTE_NAME))
                {
                    continue;
                }

                setAttribute(key, value);
            }

            setId(getAttribute(ID_ATTRIBUTE_NAME));
            setClassName(getAttribute(CLASS_ATTRIBUTE_NAME));

            invalidateGeometry();
        }
    }
}
