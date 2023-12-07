#pragma once

#include "Base.hpp"

#include "Queue.hpp"

namespace Engine
{
	namespace Runtime
	{
		namespace Renderer
		{
			namespace Device
			{
				struct BufferCreateInfo
				{
					size_t size;
					vk::BufferUsageFlags usage;
					vk::Device logicalDevice;
					vk::PhysicalDevice physicalDevice;
				};

				struct Buffer
				{
					vk::Buffer instance;
					vk::DeviceMemory memory;
				};

				void pickPhysicalDevice(vk::PhysicalDevice& outPhysicalDevice, vk::Instance& inInstance);
				void initLogicalDevice(
					vk::Device& outLogicalDevice,
					vk::PhysicalDevice& inPhysicalDevice,
					vk::SurfaceKHR& inSurface
				);
				
				uint32_t findMemoryTypeIndex(
					vk::PhysicalDevice& inPhysicalDevice,
					uint32_t inSupportedMemoryIndices,
					vk::MemoryPropertyFlags inRequestMemoryProperties
				);
				void initBuffer(Buffer& outBuffer, BufferCreateInfo& inCreateInfo);
				void allocateBuffer(Buffer& outBuffer, BufferCreateInfo& inCreateInfo);
			}
		}
	}
}