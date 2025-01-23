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
                enum class Type : std::uint8_t
                {
                    Color,
                    Depth
                };

            public:
                Type                 type;
                vk::Format           format;
                vk::AttachmentLoadOp loadOp;
                vk::ImageLayout      initialLayout;
                vk::ImageLayout      finalLayout;
            };
        }
    }
}