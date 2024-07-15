#pragma once

#include "Runtime/Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace ListComponent
        {
            const std::string TAG_ID = "List";

            // Attributes
            const std::string BACKGROUND_COLOR_ATTRIBUTE_NAME = "backgroundColor";
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
            ImVec4 getBackgroundColor(const pugi::xml_node& inNode);

            void validate(const pugi::xml_node& inNode);
            void compile(pugi::xml_node& outNode);
        }
    }
}