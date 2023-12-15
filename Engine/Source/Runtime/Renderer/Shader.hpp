#pragma once

#include "Base.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            namespace Shader
            {
                void initModule(
                    vk::ShaderModule& outShaderModule,
                    const std::string& inShaderName,
                    const vk::Device& inLogicalDevice
                );
            }
        }
    }
}