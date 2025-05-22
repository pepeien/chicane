#pragma once

// Macros
#if defined(BUILD_SHARED)
#   if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#      if defined(BUILD_EXPORT)
#          define CHICANE_BOX __declspec(dllexport)
#      else
#          define CHICANE_BOX __declspec(dllimport)
#      endif
#   elif defined(__GNUC__) && __GNUC__ >= 4
#       define CHICANE_BOX __attribute__((visibility("default")))
#   else
#       define CHICANE_BOX
#   endif
#else
#   define CHICANE_BOX
#endif

#include "Chicane/Core.hpp"