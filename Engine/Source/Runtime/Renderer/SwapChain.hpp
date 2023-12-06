#pragma once

#include "Base.hpp"

#include "Classes/Queue.hpp"
#include "Classes/SwapChain.hpp"

namespace Engine
{
	namespace Core
	{
		namespace Render
		{
			namespace SwapChain
			{
				void init(
					SwapChain::Bundle& allocator,
					vk::PhysicalDevice& inPhysicalDevice,
					vk::Device& inLogicalDevice,
					vk::SurfaceKHR& inSurface,
					int inWidth,
					int inHeight
				);
			}
		}
	}
}