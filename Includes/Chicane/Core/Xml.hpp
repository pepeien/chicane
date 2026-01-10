#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/FileSystem.hpp"

#include <pugixml.hpp>

namespace Chicane
{
    namespace Xml
    {
        CHICANE_CORE bool isEmpty(const pugi::xml_document& inDocument);
        CHICANE_CORE bool isEmpty(const pugi::xml_node& inNode);

        CHICANE_CORE void save(
            const pugi::xml_document& inDocument,
            const FileSystem::Path&   inFilepath
        );
        CHICANE_CORE pugi::xml_document
        load(const FileSystem::Path& inFilepath);

        CHICANE_CORE pugi::xml_attribute
        getAttribute(const String& inName, const pugi::xml_node& inNode);

        CHICANE_CORE void
        addText(pugi::xml_node& outNode, const String& inText);
    }
}