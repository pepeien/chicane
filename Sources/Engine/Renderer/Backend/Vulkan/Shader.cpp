#include "Chicane/Renderer/Backend/Vulkan/Shader.hpp"

#include "Chicane/Core/FileSystem.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace VulkanShader
        {
            vk::PipelineShaderStageCreateInfo initShaderStage(
                const vk::Device& inLogicalDevice, const VulkanShaderStageCreateInfo& inCreateInfo
            )
            {
                const std::vector<char> shaderCode = FileSystem::read(inCreateInfo.path.toStandard());

                vk::ShaderModuleCreateInfo moduleInfo;
                moduleInfo.flags    = vk::ShaderModuleCreateFlags();
                moduleInfo.codeSize = shaderCode.size();
                moduleInfo.pCode    = reinterpret_cast<const std::uint32_t*>(shaderCode.data());

                vk::PipelineShaderStageCreateInfo stageInfo;
                stageInfo.flags  = vk::PipelineShaderStageCreateFlags();
                stageInfo.stage  = inCreateInfo.type;
                stageInfo.module = inLogicalDevice.createShaderModule(moduleInfo);
                stageInfo.pName  = "main";

                return stageInfo;
            }
        }
    }
}