#pragma once

#include "Base.hpp"

namespace Engine
{
	namespace Runtime
	{
		namespace Renderer
		{
			namespace Surface
			{
				void init(vk::SurfaceKHR& allocator, vk::Instance& inInstance, GLFWwindow* inWindow);
			}
		}
	}
}