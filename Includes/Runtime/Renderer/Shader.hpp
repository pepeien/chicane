#pragma once

#include "Runtime/Runtime.hpp"
#include "Runtime/Core/FileSystem.hpp"

namespace Chicane
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