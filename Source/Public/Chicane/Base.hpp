#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#define SDL_MAIN_HANDLED

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION

#ifdef NDEBUG
    #define IS_DEBUGGING false
#else
    #define IS_DEBUGGING true
#endif

#ifdef _unix__
    #define OS_LINUX
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    #define OS_WINDOWS
#endif

// System
#include <algorithm>
#include <array>
#include <condition_variable>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <math.h>
#include <memory>
#include <mutex>
#include <ostream>
#include <optional>
#include <deque>
#include <set>
#include <string>
#include <thread>
#include <vector>
#include <unordered_map>

// Vendor
#include "base64.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"

#include "pugixml.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/backends/imgui_impl_sdl3.h"
#include "imgui/backends/imgui_impl_vulkan.h"
#include "imgui/misc/cpp/imgui_stdlib.h"

#include "SDL3/SDL.h"
#include "SDL3/SDL_audio.h"

#include "stb/stb_image.h"

// Settings
static constexpr const char* APPLICATION_NAME = "Chicane Engine";
static constexpr const char* ENGINE_NAME      = "Chicane";

// Z-Up Right Handed
static constexpr glm::vec3 UP_DIRECTION      = glm::vec3(0.0f, 0.0f, 1.0f);
static constexpr glm::vec3 RIGHT_DIRECTION   = glm::vec3(1.0f, 0.0f, 0.0f);
static constexpr glm::vec3 FORWARD_DIRECTION = glm::vec3(0.0f, 1.0f, 0.0f);