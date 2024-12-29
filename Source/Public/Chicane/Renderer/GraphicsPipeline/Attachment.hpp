#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace GraphicsPipeline
    {
        struct Attachment
        {
        public:
            vk::Format           format;
            vk::AttachmentLoadOp loadOp;
            vk::ImageLayout      initialLayout;
        };
    }
}