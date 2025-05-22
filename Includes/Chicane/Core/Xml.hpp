#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    namespace XML
    {
        CHICANE_CORE bool isEmpty(const pugi::xml_document& inDocument);
        CHICANE_CORE bool isEmpty(const pugi::xml_node& inNode);

        CHICANE_CORE void save(const pugi::xml_document& inDocument, const std::string& inFilepath);
        CHICANE_CORE void load(pugi::xml_document& outDocument, const std::string& inFilepath);

        CHICANE_CORE pugi::xml_attribute getAttribute(
            const std::string& inName,
            const pugi::xml_node& inNode
        );

        CHICANE_CORE void addText(pugi::xml_node& outNode, const std::string& inText);
    }
}