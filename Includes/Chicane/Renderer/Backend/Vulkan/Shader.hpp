#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Shader/CreateInfo.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace VulkanShader
        {
            CHICANE_RENDERER vk::PipelineShaderStageCreateInfo initShaderStage(
                const vk::Device& inLogicalDevice, const VulkanShaderStageCreateInfo& inCreateInfo
            );
        }
    }
}