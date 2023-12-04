#pragma once

#include "Base.h"

namespace Engine
{
    namespace Core
    {
        namespace Vulkan
        {
			namespace Mounter
			{
				void initShaderModule(vk::ShaderModule& allocator, const std::string& inShaderName, vk::Device& inLogicalDevice);
			}
		}
	}
}