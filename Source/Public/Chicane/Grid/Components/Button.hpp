#pragma once

#include "Chicane/Grid/Essential.hpp"
#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace ButtonComponent
        {
            const std::string TAG_ID = "Button";

            // Attributes
            const std::string ON_CLICK_ATTRIBUTE_NAME = "onClick";
            const std::string ON_CLICK_EVENT_KEYWORD  = "$event";

            // Props
            struct Props
            {
                std::string id = ""; // Required
                Style style = {}; // Optional
                ComponentChildren children = ComponentChildren(pugi::xml_node_iterator(), pugi::xml_node_iterator()); // Optional

                // onClick
                ComponentEvent onClickEvent = {}; // Optional
                ComponentFunction onClick; // Optional
            };

            void validate(const Props& inProps);

            void compileRaw(const Props& inProps);
            void compile(const pugi::xml_node& inNode);
        }
    }
}