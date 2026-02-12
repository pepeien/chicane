#include "Chicane/Box/Asset/Reference.hpp"

namespace Chicane
{
    namespace Box
    {
        AssetReference::AssetReference()
            : m_source(""),
              m_reference("")
        {}

        bool AssetReference::isValid() const
        {
            return FileSystem::exists(m_source);
        }

        void AssetReference::saveTo(pugi::xml_node& outNode) const
        {
            Xml::addAttribute(outNode, ASSET_SOURCE_ATTRIBUTE_NAME, getSource());
            Xml::addText(outNode, getReference());
        }

        void AssetReference::setFrom(const pugi::xml_node& inNode)
        {
            setSource(Xml::getAttribute(ASSET_SOURCE_ATTRIBUTE_NAME, inNode).as_string());
            setReference(inNode.child_value());
        }

        const FileSystem::Path& AssetReference::getSource() const
        {
            return m_source;
        }

        void AssetReference::setSource(const FileSystem::Path& inValue)
        {
            m_source = inValue;
        }

        const String& AssetReference::getReference() const
        {
            return m_reference;
        }

        void AssetReference::setReference(const String& inValue)
        {
            m_reference = inValue;
        }
    }
}