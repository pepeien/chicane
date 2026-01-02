#pragma once

// Macros
#if defined(BUILD_SHARED)
#   if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#      if defined(BUILD_EXPORT)
#          define CHICANE_SPRING __declspec(dllexport)
#      else
#          define CHICANE_SPRING __declspec(dllimport)
#      endif
#   elif defined(__GNUC__) || defined(__clang__)
#       define CHICANE_SPRING __attribute__((visibility("default")))
#   else
#       define CHICANE_SPRING
#   endif
#else
#   define CHICANE_SPRING
#endif

#include "Chicane/Core.hpp"