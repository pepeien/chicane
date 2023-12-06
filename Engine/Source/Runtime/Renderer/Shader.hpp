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
				void initModule(vk::ShaderModule& allocator, const std::string& inShaderName, vk::Device& inLogicalDevice);
			}
		}
	}
}