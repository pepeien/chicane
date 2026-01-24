#pragma once

#include <cstdint>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        enum class VulkanGraphicsPipelineAttachmentType : std::uint8_t
        {
            Color,
            Depth
        };
    }
}