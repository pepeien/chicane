#pragma once

#include "Base.hpp"

namespace Engine
{
	namespace Core
	{
		namespace Render
		{
			namespace Instance
			{
				void init(vk::Instance& allocator, vk::DispatchLoaderDynamic& dldiAllocator);
			}
		}
	}
}