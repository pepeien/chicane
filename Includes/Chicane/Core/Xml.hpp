#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/FileSystem.hpp"

namespace Chicane
{
    namespace Xml
    {
        CHICANE_CORE bool isEmpty(const pugi::xml_document& inDocument);
        CHICANE_CORE bool isEmpty(const pugi::xml_node& inNode);

        CHICANE_CORE void save(const pugi::xml_document& inDocument, const FileSystem::Path& inFilepath);
        CHICANE_CORE pugi::xml_document load(const FileSystem::Path& inFilepath);

        CHICANE_CORE pugi::xml_attribute getAttribute(
            const std::string& inName,
            const pugi::xml_node& inNode
        );

        CHICANE_CORE void addText(pugi::xml_node& outNode, const std::string& inText);
    }
}