#pragma once

#include "Base.hpp"

#include "Vertex.hpp"

namespace Engine
{
	namespace Runtime
	{
		namespace Renderer
		{
			class Mesh
			{
			public:
				template<typename T>
				Mesh(
					vk::Device& inLogicalDevice,
					vk::PhysicalDevice& inPhysicalDevice,
					std::vector<T>& inVertices
				)
				{
					logicalDevice  = inLogicalDevice;
					physicalDevice = inPhysicalDevice;

					Vertex::BufferCreateInfo bufferCreateInfo;
					bufferCreateInfo.physicalDevice = inPhysicalDevice;
					bufferCreateInfo.logicalDevice  = inLogicalDevice;
					bufferCreateInfo.size           = sizeof(T) * inVertices.size();
					bufferCreateInfo.usage          = vk::BufferUsageFlagBits::eVertexBuffer;

					Vertex::initBuffer(vertexBuffer, bufferCreateInfo);

					void* memoryLocation = logicalDevice.mapMemory(vertexBuffer.memory, 0, bufferCreateInfo.size);
					memcpy(memoryLocation, inVertices.data(), bufferCreateInfo.size);

					logicalDevice.unmapMemory(vertexBuffer.memory);
				}

				~Mesh()
				{
					logicalDevice.destroyBuffer(vertexBuffer.instance);
					logicalDevice.freeMemory(vertexBuffer.memory);
				}

			public:
				Vertex::Buffer vertexBuffer;

			protected:
				vk::Device logicalDevice;
				vk::PhysicalDevice physicalDevice;
			};
		}
	}
}