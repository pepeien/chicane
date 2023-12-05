#pragma once

#include "Base.hpp"

namespace Engine
{
	namespace Core
	{
		namespace Render
		{
			namespace Surface
			{
				void init(vk::SurfaceKHR& allocator, vk::Instance& inInstance, GLFWwindow* inWindow);
			}
		}
	}
}