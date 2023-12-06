#pragma once

#include "Base.hpp"

namespace Engine
{
	namespace Runtime
	{
		namespace Renderer
		{
			namespace Instance
			{
				void init(vk::Instance& allocator, vk::DispatchLoaderDynamic& dldiAllocator);
			}
		}
	}
}