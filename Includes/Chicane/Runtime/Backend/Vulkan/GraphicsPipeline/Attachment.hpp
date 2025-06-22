#pragma once

#include "Chicane/Runtime/Backend/Vulkan/Essential.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace GraphicsPipeline
        {
            struct CHICANE_RUNTIME Attachment
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