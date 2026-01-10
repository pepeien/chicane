#pragma once

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Shader/CreateInfo.hpp"

#include <vulkan/vulkan.hpp>

namespace Chicane
{
    namespace Vulkan
    {
        namespace Shader
        {
            CHICANE_RUNTIME vk::PipelineShaderStageCreateInfo initShaderStage(
                const vk::Device&      inLogicalDevice,
                const StageCreateInfo& inCreateInfo
            );
        }
    }
}