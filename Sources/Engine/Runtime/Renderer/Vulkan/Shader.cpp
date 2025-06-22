#include "Chicane/Runtime/Renderer/Vulkan/Shader.hpp"

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
                const std::vector<char> shaderCode = FileSystem::read(inCreateInfo.path.toStandard());

                vk::ShaderModuleCreateInfo moduleInfo = {};
                moduleInfo.flags    = vk::ShaderModuleCreateFlags();
                moduleInfo.codeSize = shaderCode.size();
                moduleInfo.pCode    = reinterpret_cast<const std::uint32_t*>(shaderCode.data());

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