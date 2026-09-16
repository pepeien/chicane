#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Xml/Node/Type.hpp"

namespace Chicane
{
    class XmlDocument;

    class CHICANE_CORE XmlNode
    {
        friend class XmlDocument;

    public:
        using Attributes = std::unordered_map<String, String>;

    public:
        XmlNode();

        bool empty() const;
        bool isEmpty() const;
        explicit operator bool() const;

        bool operator==(const XmlNode& inOther) const;
        bool operator!=(const XmlNode& inOther) const;

        XmlNodeType getType() const;
        bool isElement() const;
        bool isText() const;
        bool isCData() const;

        String getName() const;
        String getValue() const;
        String getText() const;
        void setText(const String& inText);

        XmlNode getParent() const;
        XmlNode getRoot() const;
        XmlNode getFirstChild() const;
        XmlNode getLastChild() const;
        XmlNode getNextSibling() const;
        XmlNode getNextSibling(const String& inName) const;
        XmlNode getChild(const String& inName) const;
        XmlNode findChildByAttribute(const String& inName, const String& inValue) const;

        std::vector<XmlNode> getChildren() const;
        std::vector<XmlNode> getChildren(const String& inName) const;

        XmlNode appendChild(const String& inName);
        XmlNode prependChild(const String& inName);
        XmlNode insertChildAfter(const String& inName, const XmlNode& inAfter);
        XmlNode appendCopy(const XmlNode& inNode);
        void removeChild(const XmlNode& inNode);

        bool hasAttribute(const String& inName) const;
        Attributes getAttributes() const;
        String getAttribute(const String& inName) const;
        void setAttribute(const String& inName, const String& inValue);
        void removeAttribute(const String& inName);

        float parseFloat(const String& inName, float inFallback) const;
        bool parseBool(const String& inName, bool inFallback) const;
        std::uint32_t parseUint(const String& inName, std::uint32_t inFallback) const;
        String parseString(const String& inName, const String& inFallback) const;
        Vec3 parseVec3(const String& inName, const Vec3& inFallback) const;
        Vec4 parseColor(const String& inName, const Vec4& inFallback) const;

    private:
        explicit XmlNode(void* inNode);

        void* handle() const;

    private:
        void* m_node;
    };
}
