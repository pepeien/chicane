#pragma once

#include "Chicane/Grid/Essential.hpp"
#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace PopupComponent
        {
            const std::string TAG_ID = "Popup";

            // Attributes
            const std::string IS_OPEN_ATTRIBUTE_NAME        = "isOpen";
            const std::string IS_CONSTRAINED_ATTRIBUTE_NAME = "isConstrained";

            // Props
            struct Props
            {
                std::string id             = ""; // Required
                bool isOpen                = true; // Optional
                bool isConstrained         = true; // Optional
                Style style                = {}; // Optional
                ComponentChildren children = ComponentChildren(pugi::xml_node_iterator(), pugi::xml_node_iterator()); // Optional
            };

            void compileRaw(const Props& inProps);
            void compile(const pugi::xml_node& inNode);
        }
    }
}