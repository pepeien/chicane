#pragma once

// Macros
#if defined(BUILD_SHARED)
#   if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#      if defined(BUILD_EXPORT)
#          define CHICANE_RUNTIME __declspec(dllexport)
#      else
#          define CHICANE_RUNTIME __declspec(dllimport)
#      endif
#   elif __GNUC__ >= 4
#       define __attribute__((visibility("default")))
#   else
#       define CHICANE_RUNTIME
#   endif
#else
#   define CHICANE_RUNTIME
#endif

#include "Chicane/Box.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Grid.hpp"