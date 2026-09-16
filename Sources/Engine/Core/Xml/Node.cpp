#include "Chicane/Core/Xml/Node.hpp"

#include <pugixml.hpp>

#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace
    {
        pugi::xml_node asPugi(void* inNode)
        {
            return pugi::xml_node(static_cast<pugi::xml_node_struct*>(inNode));
        }

        void* asHandle(const pugi::xml_node& inNode)
        {
            return inNode.internal_object();
        }
    }

    XmlNode::XmlNode()
        : m_node(nullptr)
    {}

    XmlNode::XmlNode(void* inNode)
        : m_node(inNode)
    {}

    bool XmlNode::empty() const
    {
        return asPugi(m_node).empty();
    }

    bool XmlNode::isEmpty() const
    {
        return empty();
    }

    XmlNode::operator bool() const
    {
        return !empty();
    }

    bool XmlNode::operator==(const XmlNode& inOther) const
    {
        return m_node == inOther.m_node;
    }

    bool XmlNode::operator!=(const XmlNode& inOther) const
    {
        return m_node != inOther.m_node;
    }

    XmlNodeType XmlNode::getType() const
    {
        switch (asPugi(m_node).type())
        {
        case pugi::node_element:
            return XmlNodeType::Element;

        case pugi::node_pcdata:
            return XmlNodeType::Pcdata;

        case pugi::node_cdata:
            return XmlNodeType::Cdata;

        case pugi::node_comment:
            return XmlNodeType::Comment;

        case pugi::node_null:
            return XmlNodeType::Null;

        default:
            return XmlNodeType::Other;
        }
    }

    bool XmlNode::isElement() const
    {
        return getType() == XmlNodeType::Element;
    }

    bool XmlNode::isText() const
    {
        return getType() == XmlNodeType::Pcdata;
    }

    bool XmlNode::isCData() const
    {
        return getType() == XmlNodeType::Cdata;
    }

    String XmlNode::getName() const
    {
        return asPugi(m_node).name();
    }

    String XmlNode::getValue() const
    {
        return asPugi(m_node).value();
    }

    String XmlNode::getText() const
    {
        return asPugi(m_node).text().as_string();
    }

    void XmlNode::setText(const String& inText)
    {
        if (inText.isEmpty())
        {
            return;
        }

        asPugi(m_node).text().set(inText.toChar(), inText.size());
    }

    XmlNode XmlNode::getParent() const
    {
        return XmlNode(asHandle(asPugi(m_node).parent()));
    }

    XmlNode XmlNode::getRoot() const
    {
        return XmlNode(asHandle(asPugi(m_node).root()));
    }

    XmlNode XmlNode::getFirstChild() const
    {
        return XmlNode(asHandle(asPugi(m_node).first_child()));
    }

    XmlNode XmlNode::getLastChild() const
    {
        return XmlNode(asHandle(asPugi(m_node).last_child()));
    }

    XmlNode XmlNode::getNextSibling() const
    {
        return XmlNode(asHandle(asPugi(m_node).next_sibling()));
    }

    XmlNode XmlNode::getNextSibling(const String& inName) const
    {
        return XmlNode(asHandle(asPugi(m_node).next_sibling(inName.toChar())));
    }

    XmlNode XmlNode::getChild(const String& inName) const
    {
        return XmlNode(asHandle(asPugi(m_node).child(inName.toChar())));
    }

    XmlNode XmlNode::findChildByAttribute(const String& inName, const String& inValue) const
    {
        return XmlNode(asHandle(asPugi(m_node).find_child_by_attribute(inName.toChar(), inValue.toChar())));
    }

    std::vector<XmlNode> XmlNode::getChildren() const
    {
        std::vector<XmlNode> result;
        for (pugi::xml_node child : asPugi(m_node).children())
        {
            result.emplace_back(XmlNode(asHandle(child)));
        }

        return result;
    }

    std::vector<XmlNode> XmlNode::getChildren(const String& inName) const
    {
        std::vector<XmlNode> result;
        for (pugi::xml_node child : asPugi(m_node).children(inName.toChar()))
        {
            result.emplace_back(XmlNode(asHandle(child)));
        }

        return result;
    }

    XmlNode XmlNode::appendChild(const String& inName)
    {
        return XmlNode(asHandle(asPugi(m_node).append_child(inName.toChar())));
    }

    XmlNode XmlNode::prependChild(const String& inName)
    {
        return XmlNode(asHandle(asPugi(m_node).prepend_child(inName.toChar())));
    }

    XmlNode XmlNode::insertChildAfter(const String& inName, const XmlNode& inAfter)
    {
        return XmlNode(asHandle(asPugi(m_node).insert_child_after(inName.toChar(), asPugi(inAfter.m_node))));
    }

    XmlNode XmlNode::appendCopy(const XmlNode& inNode)
    {
        return XmlNode(asHandle(asPugi(m_node).append_copy(asPugi(inNode.m_node))));
    }

    void XmlNode::removeChild(const XmlNode& inNode)
    {
        asPugi(m_node).remove_child(asPugi(inNode.m_node));
    }

    bool XmlNode::hasAttribute(const String& inName) const
    {
        return !asPugi(m_node).attribute(inName.toChar()).empty();
    }

    XmlNode::Attributes XmlNode::getAttributes() const
    {
        Attributes result;
        for (pugi::xml_attribute attribute : asPugi(m_node).attributes())
        {
            result.insert(std::make_pair(attribute.name(), attribute.as_string()));
        }

        return result;
    }

    String XmlNode::getAttribute(const String& inName) const
    {
        return asPugi(m_node).attribute(inName.toChar()).as_string();
    }

    void XmlNode::setAttribute(const String& inName, const String& inValue)
    {
        if (inName.isEmpty())
        {
            return;
        }

        pugi::xml_node      node      = asPugi(m_node);
        pugi::xml_attribute attribute = node.attribute(inName.toChar());
        if (attribute.empty())
        {
            attribute = node.append_attribute(inName.toChar());
        }

        attribute.set_value(inValue.toStandard());
    }

    void XmlNode::removeAttribute(const String& inName)
    {
        asPugi(m_node).remove_attribute(inName.toChar());
    }

    float XmlNode::parseFloat(const String& inName, float inFallback) const
    {
        return Xml::parseFloat(*this, inName, inFallback);
    }

    bool XmlNode::parseBool(const String& inName, bool inFallback) const
    {
        return Xml::parseBool(*this, inName, inFallback);
    }

    std::uint32_t XmlNode::parseUint(const String& inName, std::uint32_t inFallback) const
    {
        return Xml::parseUint(*this, inName, inFallback);
    }

    String XmlNode::parseString(const String& inName, const String& inFallback) const
    {
        return Xml::parseString(*this, inName, inFallback);
    }

    Vec3 XmlNode::parseVec3(const String& inName, const Vec3& inFallback) const
    {
        return Xml::parseVec3(*this, inName, inFallback);
    }

    Vec4 XmlNode::parseColor(const String& inName, const Vec4& inFallback) const
    {
        return Xml::parseColor(*this, inName, inFallback);
    }

    void* XmlNode::handle() const
    {
        return m_node;
    }
}
