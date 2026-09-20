#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanRHIPassObjects
        {
        public:
            vk::RenderPass  pass        = nullptr;
            vk::Framebuffer framebuffer = nullptr;
        };
    }
}
