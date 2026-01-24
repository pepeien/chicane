#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/GraphicsPipeline/Attachment/Type.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanGraphicsPipelineAttachment
        {
        public:
            VulkanGraphicsPipelineAttachmentType type;
            vk::Format                           format;
            vk::AttachmentLoadOp                 loadOp;
            vk::ImageLayout                      initialLayout;
            vk::ImageLayout                      finalLayout;
        };
    }
}