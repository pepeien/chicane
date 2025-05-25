#pragma once

// Macros
#if defined(BUILD_SHARED)
#   if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#      if defined(BUILD_EXPORT)
#          define CHICANE_GRID __declspec(dllexport)
#      else
#          define CHICANE_GRID __declspec(dllimport)
#      endif
#   elif defined(__GNUC__) && __GNUC__ >= 4
#       define CHICANE_GRID __attribute__((visibility("default")))
#   else
#       define CHICANE_GRID
#   endif
#else
#   define CHICANE_GRID
#endif

#include "Chicane/Box.hpp"
#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Grid
    {
        // Ref Value
        static constexpr inline const char* REFERENCE_VALUE_OPENING = "{{";
        static constexpr inline const char* REFERENCE_VALUE_CLOSING = "}}";

        static constexpr inline const char FUNCTION_PARAMS_OPENING   = '(';
        static constexpr inline const char FUNCTION_PARAMS_SEPARATOR = ',';
        static constexpr inline const char FUNCTION_PARAMS_CLOSING   = ')';

        // Value attributes
        static constexpr inline const char* ID_ATTRIBUTE_NAME    = "id";
        static constexpr inline const char* CLASS_ATTRIBUTE_NAME = "class";

        // Lifecycle attributes
        static constexpr inline const char* ON_TICK_ATTRIBUTE = "onTick";

        // List
        static constexpr inline const char* ITEMS_ATTRIBUTE_NAME       = "items";
        static constexpr inline const char* ITEM_GETTER_ATTRIBUTE_NAME = "itemGetter";

        CHICANE_GRID std::string extractParams(const std::string& inValue);
    }
}