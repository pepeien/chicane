#include "Backend/Vulkan/GraphicsPipeline/Attachment/Type.hpp"

namespace Chicane
{
    String toString(Renderer::VulkanGraphicsPipelineAttachmentType inValue)
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
