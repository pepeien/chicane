#pragma once

#include "Base.hpp"

namespace Chicane
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