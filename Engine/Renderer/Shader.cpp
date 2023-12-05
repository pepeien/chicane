#include "Shader.hpp"

namespace Engine
{
    namespace Core
    {
        namespace Render
        {
			namespace Shader
			{
				void initModule(vk::ShaderModule& allocator, const std::string& inShaderName, vk::Device& inLogicalDevice)
				{
					std::vector<char> shaderCode = FileSystem::readShader(inShaderName);

					vk::ShaderModuleCreateInfo moduleInfo = {};
					moduleInfo.flags    = vk::ShaderModuleCreateFlags();
					moduleInfo.codeSize = shaderCode.size();
					moduleInfo.pCode    = reinterpret_cast<const uint32_t *>(shaderCode.data());

					allocator = inLogicalDevice.createShaderModule(moduleInfo);
				}
			}
		}
	}
}