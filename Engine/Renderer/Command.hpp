#pragma once

#include "Classes/Buffer.hpp"
#include "Classes/Queue.hpp"

namespace Engine
{
	namespace Core
	{
		namespace Render
		{
			namespace Command
			{
				void initPool(
					vk::CommandPool& allocator,
					vk::Device& inLogicalDevice,
					vk::PhysicalDevice& inPhysicalDevice,
					vk::SurfaceKHR& inSurface
				);

				void initBuffers(vk::CommandBuffer& allocator, Buffer::CommandBufferCreateInfo& createInfo);
			}
		}
	}
}