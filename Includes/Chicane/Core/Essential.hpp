#pragma once

// Macros
#ifdef NDEBUG
#   define IS_DEBUGGING false
#else
#   define IS_DEBUGGING true
#endif

#if defined(BUILD_SHARED)
#   if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#      if defined(BUILD_EXPORT)
#          define CHICANE_CORE __declspec(dllexport)
#      else
#          define CHICANE_CORE __declspec(dllimport)
#      endif
#   elif __GNUC__ >= 4
#       define __attribute__((visibility("default")))
#   else
#       define CHICANE_CORE
#   endif
#else
#   define CHICANE_CORE
#endif

// System
#include <algorithm>
#include <array>
#include <condition_variable>
#include <cstdlib>
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

#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>

// Settings
static constexpr const char* APPLICATION_NAME = "Chicane Engine";
static constexpr const char* ENGINE_NAME      = "Chicane";

// Z-Up Right Handed
static constexpr glm::vec3 UP_DIRECTION      = glm::vec3(0.0f, 0.0f, 1.0f);
static constexpr glm::vec3 RIGHT_DIRECTION   = glm::vec3(1.0f, 0.0f, 0.0f);
static constexpr glm::vec3 FORWARD_DIRECTION = glm::vec3(0.0f, 1.0f, 0.0f);