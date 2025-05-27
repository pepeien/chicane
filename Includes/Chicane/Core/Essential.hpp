#pragma once

// Macros
#ifdef NDEBUG
#   define IS_DEBUGGING false
#else
#   define IS_DEBUGGING true
#endif

#if defined(BUILD_SHARED)
#   if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#      if defined(BUILD_EXPORT)
#          define CHICANE_CORE __declspec(dllexport)
#      else
#          define CHICANE_CORE __declspec(dllimport)
#      endif
#   elif defined(__GNUC__) && __GNUC__ >= 4
#       define CHICANE_CORE __attribute__((visibility("default")))
#   else
#       define CHICANE_CORE
#   endif
#else
#   define CHICANE_CORE
#endif

// System
#include <algorithm>
#include <array>
#include <atomic>
#include <condition_variable>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <math.h>
#include <memory>
#include <mutex>
#include <ostream>
#include <optional>
#include <deque>
#include <set>
#include <stdexcept>
#include <string>
#include <sstream>
#include <thread>
#include <vector>
#include <unordered_map>

// Vendor
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <pugixml.hpp>

// Settings
static constexpr inline const char* APPLICATION_NAME = "Chicane Engine";
static constexpr inline const char* ENGINE_NAME      = "Chicane";