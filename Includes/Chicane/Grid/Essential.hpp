#pragma once

#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Grid
    {
        // Ref Value
        static constexpr const char* REFERENCE_VALUE_OPENING = "{{";
        static constexpr const char* REFERENCE_VALUE_CLOSING = "}}";

        static constexpr const char* FUNCTION_PARAMS_OPENING   = "(";
        static constexpr const char* FUNCTION_PARAMS_SEPARATOR = ",";
        static constexpr const char* FUNCTION_PARAMS_CLOSING   = ")";

        // Value attributes
        static constexpr const char* IS_VISIBLE_ATTRIBUTE_NAME = "isVisible";
        static constexpr const char* ID_ATTRIBUTE_NAME         = "id";
        static constexpr const char* CLASS_ATTRIBUTE_NAME      = "class";

        // Lifecycle attributes
        static constexpr const char* ON_TICK_ATTRIBUTE = "onTick";

        // List
        static constexpr const char* ITEMS_ATTRIBUTE_NAME       = "items";
        static constexpr const char* ITEM_GETTER_ATTRIBUTE_NAME = "itemGetter";

        std::string extractParams(const std::string& inValue);
    }
}