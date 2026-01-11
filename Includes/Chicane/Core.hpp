#pragma once

// Macros
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#    define IS_WINDOWS true
#else
#    define IS_WINDOWS false
#endif

#if defined(__linux__) || (defined(__GNUC__) || defined(__clang__))
#    define IS_LINUX true
#else
#    define IS_LINUX false
#endif

#ifdef NDEBUG
#    define IS_DEBUGGING false
#else
#    define IS_DEBUGGING true
#endif

#if defined(BUILD_SHARED)
#    if IS_WINDOWS
#        if defined(BUILD_EXPORT)
#            define CHICANE_CORE __declspec(dllexport)
#        else
#            define CHICANE_CORE __declspec(dllimport)
#        endif
#    elif IS_LINUX
#        define CHICANE_CORE __attribute__((visibility("default")))
#    else
#        define CHICANE_CORE
#    endif
#else
#    define CHICANE_CORE
#endif

// System
#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <execution>
#include <filesystem>
#include <float.h>
#include <fstream>
#include <functional>
#include <future>
#include <iostream>
#include <map>
#include <math.h>
#include <memory>
#include <mutex>
#include <optional>
#include <ostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <vector>

// Settings
static inline constexpr const char* APPLICATION_NAME = "Chicane Engine";
static inline constexpr const char* ENGINE_NAME      = "Chicane";