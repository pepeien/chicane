#pragma once

#include "Base.hpp"

#include "Buffer.hpp"
#include "Queue.hpp"

namespace Engine
{
	namespace Runtime
	{
		namespace Renderer
		{
			namespace Command
			{
				void initPool(
					vk::CommandPool& outCommandPool,
					vk::Device& inLogicalDevice,
					vk::PhysicalDevice& inPhysicalDevice,
					vk::SurfaceKHR& inSurface
				);

				void initBuffers(vk::CommandBuffer& outCommandPool, Buffer::CommandBufferCreateInfo& inCreateInfo);
			}
		}
	}
}