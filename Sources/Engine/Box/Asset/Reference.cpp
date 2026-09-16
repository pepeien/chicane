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

        void AssetReference::saveTo(XmlNode& outNode) const
        {
            Xml::addAttribute(outNode, ASSET_SOURCE_ATTRIBUTE_NAME, FileSystem::rootRelative(getSource()).toString());
            Xml::addText(outNode, getReference());
        }

        void AssetReference::setFrom(const XmlNode& inNode, const FileSystem::Path& inBase)
        {
            setSource(FileSystem::resolve(Xml::getAttribute(ASSET_SOURCE_ATTRIBUTE_NAME, inNode), inBase));
            setReference(inNode.getText());
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