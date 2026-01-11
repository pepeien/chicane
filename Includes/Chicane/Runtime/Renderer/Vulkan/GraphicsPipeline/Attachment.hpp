#pragma once

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/GraphicsPipeline/Attachment/Type.hpp"

#include <vulkan/vulkan.hpp>

namespace Chicane
{
    namespace Vulkan
    {
        namespace GraphicsPipeline
        {
            struct CHICANE_RUNTIME Attachment
            {
            public:
                AttachmentType       type;
                vk::Format           format;
                vk::AttachmentLoadOp loadOp;
                vk::ImageLayout      initialLayout;
                vk::ImageLayout      finalLayout;
            };
        }
    }
}