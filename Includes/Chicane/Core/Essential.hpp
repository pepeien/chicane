#pragma once

// Macros
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#   define IS_WINDOWS true
#else
#   define IS_WINDOWS false
#endif

#if defined(__linux__) || (defined(__GNUC__) || defined(__clang__))
#   define IS_LINUX true
#else
#   define IS_LINUX false
#endif

#ifdef NDEBUG
#   define IS_DEBUGGING false
#else
#   define IS_DEBUGGING true
#endif

#if defined(BUILD_SHARED)
#   if IS_WINDOWS
#      if defined(BUILD_EXPORT)
#          define CHICANE_CORE __declspec(dllexport)
#      else
#          define CHICANE_CORE __declspec(dllimport)
#      endif
#   elif IS_LINUX
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
#include <execution>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <iostream>
#include <map>
#include <math.h>
#include <memory>
#include <mutex>
#include <ostream>
#include <optional>
#include <deque>
#include <type_traits>
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
static inline constexpr const char* APPLICATION_NAME = "Chicane Engine";
static inline constexpr const char* ENGINE_NAME      = "Chicane";