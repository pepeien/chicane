#pragma once

#include "Base.hpp"

namespace Engine
{
    namespace Core
    {
        namespace Render
        {
			namespace Shader
			{
				void initModule(vk::ShaderModule& allocator, const std::string& inShaderName, vk::Device& inLogicalDevice);
			}
		}
	}
}