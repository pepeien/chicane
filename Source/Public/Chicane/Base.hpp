#pragma once

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION

#define GLM_ENABLE_EXPERIMENTAL

#define SDL_MAIN_HANDLED

#define APPLICATION_NAME "Chicane Engine"
#define ENGINE_NAME      "Chicane"

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
#include <any>
#include <array>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <math.h>
#include <memory>
#include <ostream>
#include <optional>
#include <set>
#include <string>
#include <vector>
#include <unordered_map>

// Vendor
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"

#include "pugixml.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/backends/imgui_impl_sdl2.h"
#include "imgui/backends/imgui_impl_vulkan.h"
#include "imgui/misc/cpp/imgui_stdlib.h"

#include "stb/stb_image.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_vulkan.h"

#include "SDL_image.h"

#include "vulkan/vulkan.hpp"

// Consts
static const std::vector<const char*> LAYERS = {
    "VK_LAYER_KHRONOS_validation"
};

static const std::vector<const char*> DEVICE_EXTENSIONS = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

constexpr glm::vec3 FORWARD_DIRECTION = glm::vec3(1.0f,  0.0f, 0.0f);
constexpr glm::vec3 RIGHT_DIRECTION   = glm::vec3(0.0f, -1.0f, 0.0f);
constexpr glm::vec3 UP_DIRECTION      = glm::vec3(0.0f,  0.0f, 1.0f);

constexpr glm::vec3 CAMERA_FORWARD_DIRECTION = glm::vec3(0.0f, 0.0f, -1.0f);
constexpr glm::vec3 CAMERA_RIGHT_DIRECTION   = glm::vec3(1.0f, 0.0f,  0.0f);
constexpr glm::vec3 CAMERA_UP_DIRECTION      = glm::vec3(0.0f, 1.0f,  0.0f);

namespace Chicane
{
    struct FrameTelemetry
    {
        uint32_t count = 0;
        uint32_t rate  = 0;
        float time     = 0.0f;
    };

    struct Telemetry
    {
    public:
        std::clock_t delta = 0;

        FrameTelemetry frame {};

    public:
        float deltaToMs()
        {
            return (delta / (float)CLOCKS_PER_SEC) * 1000.0f;
        }

        float deltaToTick()
        {
            return delta / 1000.0f;
        }
    };
}