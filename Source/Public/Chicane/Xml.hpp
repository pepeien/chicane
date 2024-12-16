#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace XML
    {
        bool isEmpty(const pugi::xml_document& inDocument);
        bool isEmpty(const pugi::xml_node& inNode);

        void save(const pugi::xml_document& inDocument, const std::string& inFilepath);
        void load(pugi::xml_document& outDocument, const std::string& inFilepath);

        pugi::xml_attribute getAttribute(
            const std::string& inName,
            const pugi::xml_node& inNode
        );

        void addText(pugi::xml_node& outNode, const std::string& inText);
    }
}