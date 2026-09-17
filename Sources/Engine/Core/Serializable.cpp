#include "Chicane/Core/Serializable.hpp"

namespace Chicane
{
    Serializable::Serializable()
        : m_filepath(),
          m_document(),
          m_source(),
          m_attributes({}),
          m_watchers({})
    {}

    void Serializable::load(const FileSystem::Path& inFilepath)
    {
        setFilepath(inFilepath);

        m_document = XmlDocument::load(inFilepath);

        bindSource(m_document.getFirstChild());

        onAttributeSync();
    }

    void Serializable::onAttributeSync()
    {
        syncAttributes();
    }

    void Serializable::onAttributeChange(const String&, const String&)
    {
        return;
    }

    void Serializable::save()
    {
        save(m_filepath);
    }

    void Serializable::save(const FileSystem::Path& inFilepath)
    {
        setFilepath(inFilepath);
        writeAttributesToSource();
        m_document.save(inFilepath);
    }

    void Serializable::parse(const XmlNode& inNode)
    {
        m_document.reset();
        bindSource(inNode.isEmpty() ? XmlNode() : m_document.appendCopy(inNode));
        onAttributeSync();
    }

    XmlNode Serializable::createRoot(const String& inName)
    {
        m_document.reset();
        bindSource(m_document.appendChild(inName));
        onAttributeSync();

        return m_source;
    }

    const FileSystem::Path& Serializable::getFilepath() const
    {
        return m_filepath;
    }

    void Serializable::setFilepath(const FileSystem::Path& inFilepath)
    {
        m_filepath = inFilepath;
    }

    const XmlDocument& Serializable::getDocument() const
    {
        return m_document;
    }

    XmlDocument& Serializable::getDocument()
    {
        return m_document;
    }

    const XmlNode& Serializable::getSource() const
    {
        return m_source;
    }

    XmlNode& Serializable::getSource()
    {
        return m_source;
    }

    const Serializable::AttributeMap& Serializable::getAttributes() const
    {
        return m_attributes;
    }

    bool Serializable::hasAttribute(const String& inName) const
    {
        return m_attributes.find(inName) != m_attributes.end();
    }

    const String& Serializable::getAttribute(const String& inName) const
    {
        const auto found = m_attributes.find(inName);
        if (found == m_attributes.end())
        {
            return String::empty();
        }

        return found->second;
    }

    void Serializable::setAttribute(const String& inName, const String& inValue)
    {
        if (inName.isEmpty())
        {
            return;
        }

        const auto found = m_attributes.find(inName);
        if (found != m_attributes.end() && found->second.equals(inValue))
        {
            return;
        }

        m_attributes[inName] = inValue;
        if (!m_source.isEmpty())
        {
            m_source.setAttribute(inName, inValue);
        }

        emitAttribute(inName, inValue);
    }

    void Serializable::removeAttribute(const String& inName)
    {
        if (!hasAttribute(inName))
        {
            return;
        }

        m_attributes.erase(inName);
        if (!m_source.isEmpty())
        {
            m_source.removeAttribute(inName);
        }

        emitAttribute(inName, String::empty());
    }

    float Serializable::getFloat(const String& inName, float inFallback) const
    {
        if (!hasAttribute(inName))
        {
            return inFallback;
        }

        return Xml::parseFloat(getAttribute(inName), inFallback);
    }

    bool Serializable::getBool(const String& inName, bool inFallback) const
    {
        if (!hasAttribute(inName))
        {
            return inFallback;
        }

        return Xml::parseBool(getAttribute(inName), inFallback);
    }

    std::uint32_t Serializable::getUint(const String& inName, std::uint32_t inFallback) const
    {
        if (!hasAttribute(inName))
        {
            return inFallback;
        }

        return Xml::parseUint(getAttribute(inName), inFallback);
    }

    String Serializable::getString(const String& inName, const String& inFallback) const
    {
        if (!hasAttribute(inName))
        {
            return inFallback;
        }

        return getAttribute(inName);
    }

    Vec3 Serializable::getVec3(const String& inName, const Vec3& inFallback) const
    {
        if (!hasAttribute(inName))
        {
            return inFallback;
        }

        return Xml::parseVec3(getAttribute(inName), inFallback);
    }

    Vec4 Serializable::getColor(const String& inName, const Vec4& inFallback) const
    {
        if (!hasAttribute(inName))
        {
            return inFallback;
        }

        return Xml::parseColor(getAttribute(inName), inFallback);
    }

    Serializable::AttributeSubscription Serializable::watchAttribute(
        const String& inName, const AttributeCallback& inCallback
    )
    {
        return m_watchers[inName].subscribe(inCallback).next(getAttribute(inName));
    }

    void Serializable::bindSource(const XmlNode& inNode)
    {
        m_source = inNode;
    }

    void Serializable::syncAttributes()
    {
        const AttributeMap previous = m_attributes;
        const AttributeMap next     = m_source.isEmpty() ? AttributeMap{} : m_source.getAttributes();

        m_attributes = next;

        for (const auto& [name, value] : previous)
        {
            if (next.find(name) != next.end())
            {
                continue;
            }

            emitAttribute(name, String::empty());
        }

        for (const auto& [name, value] : next)
        {
            const auto found = previous.find(name);
            if (found != previous.end() && found->second.equals(value))
            {
                continue;
            }

            emitAttribute(name, value);
        }
    }

    void Serializable::emitAttribute(const String& inName, const String& inValue)
    {
        const auto found = m_watchers.find(inName);
        if (found != m_watchers.end())
        {
            found->second.next(inValue);
        }

        onAttributeChange(inName, inValue);
    }

    void Serializable::writeAttributesToSource()
    {
        if (m_source.isEmpty())
        {
            return;
        }

        for (const auto& [name, value] : m_attributes)
        {
            m_source.setAttribute(name, value);
        }
    }
}
