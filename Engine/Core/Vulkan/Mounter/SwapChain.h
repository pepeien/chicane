#pragma once

#include "Base.h"

#include "Core/Vulkan/SwapChain.h"

namespace Engine
{
	namespace Core
	{
		namespace Vulkan
		{
			namespace Mounter
			{
				void initSwapChain(
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