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
        static inline constexpr const char* REFERENCE_VALUE_OPENING = "{{";
        static inline constexpr const char* REFERENCE_VALUE_CLOSING = "}}";

        static inline constexpr const char FUNCTION_PARAMS_OPENING   = '(';
        static inline constexpr const char FUNCTION_PARAMS_SEPARATOR = ',';
        static inline constexpr const char FUNCTION_PARAMS_CLOSING   = ')';

        // Value attributes
        static inline constexpr const char* ID_ATTRIBUTE_NAME    = "id";
        static inline constexpr const char* CLASS_ATTRIBUTE_NAME = "class";

        // Lifecycle attributes
        static inline constexpr const char* ON_TICK_ATTRIBUTE = "onTick";

        // List
        static inline constexpr const char* ITEMS_ATTRIBUTE_NAME       = "items";
        static inline constexpr const char* ITEM_GETTER_ATTRIBUTE_NAME = "itemGetter";
    }
}