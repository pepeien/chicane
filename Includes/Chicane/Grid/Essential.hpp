#pragma once

#include "Core.hpp"

namespace Chicane
{
    namespace Grid
    {
        // Ref Value
        static constexpr const char* REF_VALUE_OPENING = "{{";
        static constexpr const char* REF_VALUE_CLOSING = "}}";

        static constexpr char FUNCTION_PARAMS_OPENING = '(';
        static constexpr char FUNCTION_PARAMS_CLOSING = ')';

        // Value attributes
        static constexpr const char* IS_VISIBLE_ATTRIBUTE_NAME = "isVisible";
        static constexpr const char* ID_ATTRIBUTE_NAME         = "id";
        static constexpr const char* CLASS_ATTRIBUTE_NAME      = "class";

        // Lifecycle attributes
        static constexpr const char* ON_TICK_ATTRIBUTE = "onTick";

        // List
        static constexpr const char* ITEMS_ATTRIBUTE_NAME       = "items";
        static constexpr const char* ITEM_GETTER_ATTRIBUTE_NAME = "itemGetter";

        // Size units
        static constexpr const char* AUTO_SIZE_UNIT            = "auto";
        static constexpr const char* PIXEL_SIZE_UNIT           = "px";
        static constexpr const char* PERCENTAGE_SIZE_UNIT      = "%";
        static constexpr const char* VIEWPORT_HEIGHT_SIZE_UNIT = "vh";
        static constexpr const char* VIEWPORT_WIDTH_SIZE_UNIT  = "vw";

        CHICANE Vec<4, std::uint32_t> hexToRgba(const std::string& inColor);
        CHICANE Vec<3, std::uint32_t> hexToRgb(const std::string& inColor);
    }
}