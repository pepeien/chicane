#pragma once

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION

#define SDL_MAIN_HANDLED

#define APPLICATION_NAME "Chicane Engine"
#define ENGINE_NAME      "Chicane"

#ifdef NDEBUG
    #define IS_DEBUGGING false
#else
    #define IS_DEBUGGING true
#endif

// System
#include <algorithm>
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

#include "pugixml.hpp"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl2.h"
#include "imgui/backends/imgui_impl_vulkan.h"

#include "stb/stb_image.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_vulkan.h"

#include "vulkan/vulkan.hpp"

// Consts
static const std::vector<const char*> VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation"
};

static const std::vector<const char*> DEVICE_EXTENSIONS = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static const uint32_t MAX_BUFFER_MULTIPLIER = 3;

namespace Chicane
{
    struct Stats
    {
        int count            = 0;
        float time           = 0.0f;
        uint64_t framerate   = 0;
        uint64_t lastTime    = 0;
        uint64_t currentTime = 0;
    };

    struct Vec2
    {
        int x = -1;
        int y = -1;
    };
}