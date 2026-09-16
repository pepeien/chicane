#pragma once

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Xml/Document.hpp"
#include "Chicane/Core/Xml/Node.hpp"

namespace Chicane
{
    namespace Xml
    {
        using Attributes = XmlNode::Attributes;

        CHICANE_CORE bool isEmpty(const XmlDocument& inDocument);
        CHICANE_CORE bool isEmpty(const XmlNode& inNode);

        CHICANE_CORE void save(const XmlDocument& inDocument, const FileSystem::Path& inFilepath);
        CHICANE_CORE XmlDocument load(const FileSystem::Path& inFilepath);

        CHICANE_CORE Attributes getAttributes(const XmlNode& inNode);
        CHICANE_CORE String getAttribute(const String& inName, const XmlNode& inNode);
        CHICANE_CORE void addAttribute(XmlNode& outNode, const String& inName, const String& inValue);
        CHICANE_CORE void addText(XmlNode& outNode, const String& inText);

        CHICANE_CORE float parseFloat(const String& inValue, float inFallback);
        CHICANE_CORE bool parseBool(const String& inValue, bool inFallback);
        CHICANE_CORE int parseInt(const String& inValue, int inFallback);
        CHICANE_CORE std::uint32_t parseUint(const String& inValue, std::uint32_t inFallback);
        CHICANE_CORE Vec3 parseVec3(const String& inValue, const Vec3& inFallback);
        CHICANE_CORE Vec4 parseColor(const String& inValue, const Vec4& inFallback);

        CHICANE_CORE float parseFloat(const XmlNode& inNode, const String& inName, float inFallback);
        CHICANE_CORE bool parseBool(const XmlNode& inNode, const String& inName, bool inFallback);
        CHICANE_CORE std::uint32_t parseUint(const XmlNode& inNode, const String& inName, std::uint32_t inFallback);
        CHICANE_CORE String parseString(const XmlNode& inNode, const String& inName, const String& inFallback);
        CHICANE_CORE Vec3 parseVec3(const XmlNode& inNode, const String& inName, const Vec3& inFallback);
        CHICANE_CORE Vec4 parseColor(const XmlNode& inNode, const String& inName, const Vec4& inFallback);
    }
}
