#pragma once

#define GLFW_INCLUDE_VULKAN

#define APPLICATION_NAME "Chicane Engine"

#define ENGINE_NAME        "Chicane"
#define ENGINE_DIR         "../"
#define ENGINE_SHADERS_DIR "Content/Shaders/"

#ifdef NDEBUG
#define IS_DEBUGGING false
#else
#define IS_DEBUGGING true
#endif

#include <array>
#include <fstream>
#include <iostream>
#include <math.h>
#include <ostream>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "Core/FileSystem.hpp"
#include "Core/Log.hpp"

const std::vector<const char*> VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> DEVICE_EXTENSIONS = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const uint32_t MAX_BUFFER_MULTIPLIER = 2;