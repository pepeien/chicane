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
                struct ObjectData
                {
                    glm::mat4 model;
                };

                void initModule(vk::ShaderModule& outShaderModule, const std::string& inShaderName, vk::Device& inLogicalDevice);
            }
        }
    }
}