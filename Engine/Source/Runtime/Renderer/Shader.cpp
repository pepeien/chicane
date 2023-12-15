#include "Shader.hpp"

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
                )
                {
                    std::vector<char> shaderCode = Core::FileSystem::readShader(inShaderName);

                    vk::ShaderModuleCreateInfo moduleInfo = {};
                    moduleInfo.flags    = vk::ShaderModuleCreateFlags();
                    moduleInfo.codeSize = shaderCode.size();
                    moduleInfo.pCode    = reinterpret_cast<const uint32_t *>(shaderCode.data());

                    outShaderModule = inLogicalDevice.createShaderModule(moduleInfo);
                }
            }
        }
    }
}