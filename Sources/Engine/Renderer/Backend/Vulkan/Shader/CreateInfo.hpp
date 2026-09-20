#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Core/String.hpp"

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanShaderStageCreateInfo
        {
        public:
            String                  path;
            vk::ShaderStageFlagBits type;
        };
    }
}