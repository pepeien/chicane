#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Shader
        {
            void initModule(
                vk::ShaderModule& outShaderModule,
                const std::string& inFilepath,
                const vk::Device& inLogicalDevice
            );
        }
    }
}