#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Runtime/Essential.hpp"

// Consts
static const std::vector<const char*> LAYERS = {
    "VK_LAYER_KHRONOS_validation"
};

static const std::vector<const char*> EXTENSIONS = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static constexpr inline const std::uint32_t MAX_IMAGE_COUNT = 3;