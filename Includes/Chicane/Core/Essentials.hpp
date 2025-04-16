#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION

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
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"

#include "pugixml.hpp"

#include "SDL3/SDL.h"
#include "SDL3/SDL_audio.h"

#include "stb_image.h"

// Macros
#ifdef NDEBUG
#   define IS_DEBUGGING false
#else
#   define IS_DEBUGGING true
#endif

#ifdef _unix__
#   define OS_LINUX
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#   define OS_WINDOWS
#endif

#if defined(BUILD_SHARED)
#   if defined(__WIN32__) && !defined(__CYGWIN__)
#      if (defined(_MSC_VER) || defined(__MINGW32__)) && defined(BUILD_SHARED_DLL)
#          define CHICANE __declspec(dllexport)
#      elif (defined(_MSC_VER) || defined(__MINGW32__))
#          define CHICANE __declspec(dllimport)
#      endif
#   endif
#else
#   define CHICANE
#endif

// Settings
static constexpr const char* APPLICATION_NAME = "Chicane Engine";
static constexpr const char* ENGINE_NAME      = "Chicane";

// Z-Up Right Handed
static constexpr glm::vec3 UP_DIRECTION      = glm::vec3(0.0f, 0.0f, 1.0f);
static constexpr glm::vec3 RIGHT_DIRECTION   = glm::vec3(1.0f, 0.0f, 0.0f);
static constexpr glm::vec3 FORWARD_DIRECTION = glm::vec3(0.0f, 1.0f, 0.0f);