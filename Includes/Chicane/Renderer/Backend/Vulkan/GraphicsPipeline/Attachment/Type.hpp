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

        inline String toString(VulkanGraphicsPipelineAttachmentType inValue)
        {
            switch (inValue)
            {
            case VulkanGraphicsPipelineAttachmentType::Color:
                return "Color";

            case VulkanGraphicsPipelineAttachmentType::Depth:
                return "Depth";

            default:
                return "";
            }
        }
    }
}