#pragma once

#include "Core.hpp"
#include "Grid/Component/Instance.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE List : public Component
        {
        public:
            // Tag
            static constexpr const char* TAG_ID = "List";

            // Attributes
            static constexpr const char* ITEM_WIDTH_ATTRIBUTE_NAME  = "itemWidth";
            static constexpr const char* ITEM_HEIGHT_ATTRIBUTE_NAME = "itemHeight";
        };
    }
}