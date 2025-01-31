#include "Chicane/Renderer/Vulkan/Shader.hpp"

#include "Chicane/Core/FileSystem.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Shader
        {
            
            vk::PipelineShaderStageCreateInfo initShaderStage(
                const vk::Device& inLogicalDevice,
                const StageCreateInfo& inCreateInfo
            )
            {
                std::vector<char> shaderCode = FileSystem::readFile(inCreateInfo.path);

                vk::ShaderModuleCreateInfo moduleInfo = {};
                moduleInfo.flags    = vk::ShaderModuleCreateFlags();
                moduleInfo.codeSize = shaderCode.size();
                moduleInfo.pCode    = reinterpret_cast<const std::uint32_t *>(shaderCode.data());

                vk::PipelineShaderStageCreateInfo stageInfo = {};
                stageInfo.flags  = vk::PipelineShaderStageCreateFlags();
                stageInfo.stage  = inCreateInfo.type;
                stageInfo.module = inLogicalDevice.createShaderModule(moduleInfo);
                stageInfo.pName  = "main";

                return stageInfo;
            }
        }
    }
}