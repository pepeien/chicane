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

    inline String toString(Renderer::VulkanGraphicsPipelineAttachmentType inValue)
    {
        switch (inValue)
        {
        case Renderer::VulkanGraphicsPipelineAttachmentType::Color:
            return "Color";

        case Renderer::VulkanGraphicsPipelineAttachmentType::Depth:
            return "Depth";

        default:
            return "";
        }
    }
}
