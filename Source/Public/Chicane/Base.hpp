#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#define SDL_MAIN_HANDLED

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION

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
#include <condition_variable>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <functional>
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
#include "SDL3/SDL_vulkan.h"

#include "stb/stb_image.h"

#include "vulkan/vulkan.hpp"

// Consts
static const std::vector<const char*> LAYERS = {
    "VK_LAYER_KHRONOS_validation"
};

static const std::vector<const char*> EXTENSIONS = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

constexpr std::uint32_t MAX_TEXTURE_COUNT = 100000;

// Z-Up Right Handed
constexpr glm::vec3 FORWARD_DIRECTION = glm::vec3(0.0f, 1.0f, 0.0f);
constexpr glm::vec3 RIGHT_DIRECTION   = glm::vec3(1.0f, 0.0f, 0.0f);
constexpr glm::vec3 UP_DIRECTION      = glm::vec3(0.0f, 0.0f, 1.0f);

namespace Chicane
{
    typedef unsigned char ImageData;

    struct FrameTelemetry
    {
        uint32_t count = 0;
        uint32_t rate  = 0;
        float    time  = 0.0f;
    };

    struct Telemetry
    {
    public:
        std::clock_t   delta = 0;

        FrameTelemetry frame {};

    public:
        static float deltaToMs(std::clock_t inDelta)
        {
            return (inDelta / (float)CLOCKS_PER_SEC) * 1000.0f;
        }

        static float deltaToTick(std::clock_t inDelta)
        {
            return inDelta / 1000.0f;
        }

    public:
        float deltaToMs() const
        {
            return Telemetry::deltaToMs(delta);
        }

        float deltaToTick() const
        {
            return Telemetry::deltaToTick(delta);
        }
    };
}