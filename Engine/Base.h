#pragma once

#include <array>
#include <fstream>
#include <iostream>
#include <ostream>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "Core/Log.h"

#define APPLICATION_NAME "Chicane Engine"

#define ENGINE_NAME        "Chicane"
#define ENGINE_DIR         "../../Engine/"
#define ENGINE_SHADERS_DIR "Shaders/Public/"

#ifdef NDEBUG
#define IS_DEBUGGING false
#else
#define IS_DEBUGGING true
#endif

const std::vector<const char*> VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> DEVICE_EXTENSIONS = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};