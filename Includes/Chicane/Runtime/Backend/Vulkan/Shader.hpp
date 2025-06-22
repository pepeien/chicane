#pragma once

#include "Chicane/Runtime/Backend/Vulkan/Essential.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Shader/CreateInfo.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Shader
        {
            CHICANE_RUNTIME vk::PipelineShaderStageCreateInfo initShaderStage(
                const vk::Device& inLogicalDevice,
                const StageCreateInfo& inCreateInfo
            );
        }
    }
}