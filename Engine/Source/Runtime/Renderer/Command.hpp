#pragma once

#include "Classes/Buffer.hpp"
#include "Classes/Queue.hpp"

namespace Engine
{
	namespace Runtime
	{
		namespace Renderer
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