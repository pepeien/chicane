#pragma once

#include <unordered_map>

#include <pugixml.hpp>

#include "Chicane/Core.hpp"
#include "Chicane/Core/FileSystem.hpp"

namespace Chicane
{
    namespace Xml
    {
        using Attribute = std::unordered_map<String, String>;

        CHICANE_CORE bool isEmpty(const pugi::xml_document& inDocument);
        CHICANE_CORE bool isEmpty(const pugi::xml_node& inNode);

        CHICANE_CORE void save(const pugi::xml_document& inDocument, const FileSystem::Path& inFilepath);
        CHICANE_CORE pugi::xml_document load(const FileSystem::Path& inFilepath);

        CHICANE_CORE Attribute getAttributes(const pugi::xml_node& inNode);
        CHICANE_CORE pugi::xml_attribute getAttribute(const String& inName, const pugi::xml_node& inNode);
        CHICANE_CORE void addAttribute(pugi::xml_node& outNode, const String& inName, const String& inValue);

        CHICANE_CORE void addText(pugi::xml_node& outNode, const String& inText);
    }
}