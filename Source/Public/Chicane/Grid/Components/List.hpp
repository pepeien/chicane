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
            const std::string DIRECTION_ATTRIBUTE_NAME        = "direction";
            const std::string ITEMS_ATTRIBUTE_NAME            = "items";
            const std::string ITEM_GETTER_ATTRIBUTE_NAME      = "itemGetter";

            // Direction
            const std::string DIRECTION_ROW    = "ROW";
            const std::string DIRECTION_COLUMN = "COLUMN";

            enum class Direction : uint8_t
            {
                Column,
                Row
            };

            Direction getDirection(const pugi::xml_node& inNode);
            std::vector<std::any> getItems(const pugi::xml_node& inNode);
            ComponentFunction getItemGetter(const pugi::xml_node& inNode);
            ImVec4 getBackgroundColor(const Style& inStyle);

            void validate(const pugi::xml_node& inNode);
            void compile(const pugi::xml_node& inNode);
        }
    }
}