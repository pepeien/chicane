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
				void initSurface(vk::SurfaceKHR& allocator, vk::Instance& inInstance, GLFWwindow* inWindow);
			}
		}
	}
}