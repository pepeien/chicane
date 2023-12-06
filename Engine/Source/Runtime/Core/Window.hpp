#pragma once

#include "Base.hpp"

namespace Engine
{
	namespace Runtime
	{
		namespace Core
		{
			namespace Window
			{
				GLFWwindow* init(int& widthAllocator, int& heightAllocator, const char* inWindowTitle);
			}
		}
	}
}