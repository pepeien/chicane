#pragma once

#include "Chicane/Core/Essentials.hpp"

namespace Chicane
{
    namespace XML
    {
        CHICANE bool isEmpty(const pugi::xml_document& inDocument);
        CHICANE bool isEmpty(const pugi::xml_node& inNode);

        CHICANE void save(const pugi::xml_document& inDocument, const std::string& inFilepath);
        CHICANE void load(pugi::xml_document& outDocument, const std::string& inFilepath);

        CHICANE pugi::xml_attribute getAttribute(
            const std::string& inName,
            const pugi::xml_node& inNode
        );

        CHICANE void addText(pugi::xml_node& outNode, const std::string& inText);
    }
}