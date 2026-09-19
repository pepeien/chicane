#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanRHISamplerData
        {
        public:
            vk::Sampler sampler = nullptr;
            bool        bOwned  = true;
        };
    }
}
