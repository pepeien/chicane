#pragma once

#include "Chicane/Runtime.hpp"

#include <vulkan/vulkan.hpp>

namespace Chicane
{
    namespace Vulkan
    {
        namespace GraphicsPipeline
        {
            enum class AttachmentType : std::uint8_t
            {
                Color,
                Depth
            };
        }
    }
}