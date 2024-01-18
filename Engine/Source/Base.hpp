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
#include "stb/stb_image.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

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

static const std::string ENGINE_ENV_VARIABALE = "CHICANE_SDK";

static const uint32_t MAX_BUFFER_MULTIPLIER = 3;

// Source
#include "Core/FileSystem.hpp"
#include "Core/Helper.hpp"
#include "Core/Log.hpp"

namespace Engine
{
    struct Resolution
    {
        int width  = -1;
        int height = -1;
    };

    enum class Layer : uint8_t
    {
        SKY,
        SCENE
    };
}