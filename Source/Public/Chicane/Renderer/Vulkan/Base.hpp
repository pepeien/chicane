#pragma once

#include "vulkan/vulkan.hpp"

#include "Chicane/Base.hpp"

// Consts
static const std::vector<const char*> LAYERS = {
    "VK_LAYER_KHRONOS_validation"
};

static const std::vector<const char*> EXTENSIONS = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static constexpr std::uint32_t MAX_IMAGE_COUNT = 3;
