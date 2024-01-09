#pragma once

#include "Base.hpp"

namespace Engine
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