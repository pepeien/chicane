#pragma once

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE_GRID List : public Component
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "List";

            // Attributes
            static constexpr inline const char* ITEM_WIDTH_ATTRIBUTE_NAME  = "itemWidth";
            static constexpr inline const char* ITEM_HEIGHT_ATTRIBUTE_NAME = "itemHeight";

        public:
            inline List(const pugi::xml_node& inNode)
                : Component(inNode)
            {}
        };
    }
}