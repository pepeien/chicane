#pragma once

// Macros
#if defined(BUILD_SHARED)
#   if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#      if defined(BUILD_EXPORT)
#          define CHICANE_RUNTIME __declspec(dllexport)
#      else
#          define CHICANE_RUNTIME __declspec(dllimport)
#      endif
#   elif defined(__GNUC__) || defined(__clang__)
#       define CHICANE_RUNTIME __attribute__((visibility("default")))
#   else
#       define CHICANE_RUNTIME
#   endif
#else
#   define CHICANE_RUNTIME
#endif

#include "Chicane/Box.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Grid.hpp"
#include "Chicane/Screech.hpp"