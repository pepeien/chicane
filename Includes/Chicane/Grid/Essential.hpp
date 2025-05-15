#pragma once

// Macros
#if defined(BUILD_SHARED)
#   if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#      if defined(BUILD_EXPORT)
#          define CHICANE_GRID __declspec(dllexport)
#      else
#          define CHICANE_GRID __declspec(dllimport)
#      endif
#   elif __GNUC__ >= 4
#       define __attribute__((visibility("default")))
#   else
#       define CHICANE_GRID
#   endif
#else
#   define CHICANE_GRID
#endif

#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Grid
    {
        // Ref Value
        static constexpr const char* REFERENCE_VALUE_OPENING = "{{";
        static constexpr const char* REFERENCE_VALUE_CLOSING = "}}";

        static constexpr char FUNCTION_PARAMS_OPENING   = '(';
        static constexpr char FUNCTION_PARAMS_SEPARATOR = ',';
        static constexpr char FUNCTION_PARAMS_CLOSING   = ')';

        // Value attributes
        static constexpr const char* ID_ATTRIBUTE_NAME    = "id";
        static constexpr const char* CLASS_ATTRIBUTE_NAME = "class";

        // Lifecycle attributes
        static constexpr const char* ON_TICK_ATTRIBUTE = "onTick";

        // List
        static constexpr const char* ITEMS_ATTRIBUTE_NAME       = "items";
        static constexpr const char* ITEM_GETTER_ATTRIBUTE_NAME = "itemGetter";

        CHICANE_GRID std::string extractParams(const std::string& inValue);
    }
}