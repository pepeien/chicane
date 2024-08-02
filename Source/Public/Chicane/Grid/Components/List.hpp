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

            // Attributes
            const std::string DIRECTION_ATTRIBUTE_NAME   = "direction";

            // Direction
            const std::string DIRECTION_ROW    = "ROW";
            const std::string DIRECTION_COLUMN = "COLUMN";

            enum class Direction : uint8_t
            {
                Column,
                Row
            };

            // Props
            struct Props
            {
            public:
                std::string id = ""; // Required

                Direction direction = Direction::Column; // Optional
                Style style         = {}; // Optional

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