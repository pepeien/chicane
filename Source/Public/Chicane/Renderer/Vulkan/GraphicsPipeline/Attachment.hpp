#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace GraphicsPipeline
        {
            struct Attachment
            {
            public:
                vk::Format           format;
                vk::AttachmentLoadOp loadOp;
                vk::ImageLayout      initialLayout;
                vk::ImageLayout      finalLayout;
            };
        }
    }
}