#pragma once

#include <cstdint>

#include "Chicane/Renderer.hpp"
#include "Chicane/Core/String.hpp"

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

    CHICANE_RENDERER String toString(Renderer::VulkanGraphicsPipelineAttachmentType inValue);
}
