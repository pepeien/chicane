#include "Shader.hpp"

namespace Chicane
{
    namespace Shader
    {
        void initModule(
            vk::ShaderModule& outShaderModule,
            const std::string& inFilepath,
            const vk::Device& inLogicalDevice
        )
        {
            std::vector<char> shaderCode = FileSystem::readFile(inFilepath);

            vk::ShaderModuleCreateInfo moduleInfo = {};
            moduleInfo.flags    = vk::ShaderModuleCreateFlags();
            moduleInfo.codeSize = shaderCode.size();
            moduleInfo.pCode    = reinterpret_cast<const uint32_t *>(shaderCode.data());

            outShaderModule = inLogicalDevice.createShaderModule(moduleInfo);
        }
    }
}