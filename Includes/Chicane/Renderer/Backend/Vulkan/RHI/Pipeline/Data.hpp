#pragma once

#include <cstdint>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/GraphicsPipeline.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanRHIPipelineData
        {
        public:
            VulkanGraphicsPipeline pipeline;
            std::uint32_t          pushSize   = 0;
            vk::ShaderStageFlags   pushStages = {};
        };
    }
}
