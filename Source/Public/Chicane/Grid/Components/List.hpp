#pragma once

#include "Chicane/Grid/View.hpp"
#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace ListComponent
        {
            const std::string TAG_ID = "List";

            // Props
            struct Props
            {
            public:
                std::string id = ""; // Required

                Style style = {}; // Optional

                std::vector<std::any> items = {}; // Optional
                ComponentFunction itemGetter; // Optional

                ComponentChildren children = ComponentChildren(pugi::xml_node_iterator(), pugi::xml_node_iterator()); // Optional
            };

            void validate(const Props& inProps);

            void compileRaw(const Props& inProps);
            void compile(const pugi::xml_node& inNode);
        }
    }
}