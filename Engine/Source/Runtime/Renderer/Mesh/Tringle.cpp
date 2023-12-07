#include "Tringle.hpp"

std::vector<float> vertices = {
	{
		0.0f, -0.05f, 0.0f, 1.0f, 0.0f,
		0.05f, 0.05f, 0.0f, 1.0f, 0.0f,
		-0.05f, 0.05f, 0.0f, 1.0f, 0.0f
	}
};

namespace Engine
{
	namespace Runtime
	{
		namespace Renderer
		{
			namespace Mesh
			{
				Triangle::Triangle(vk::Device& inLogicalDevice, vk::PhysicalDevice& inPhysicalDevice)
				{
					logicalDevice = inLogicalDevice;

					Device::BufferCreateInfo bufferCreateInfo;
					bufferCreateInfo.physicalDevice = inPhysicalDevice;
					bufferCreateInfo.logicalDevice  = inLogicalDevice;
					bufferCreateInfo.size           = sizeof(float) * vertices.size();
					bufferCreateInfo.usage          = vk::BufferUsageFlagBits::eVertexBuffer;

					Device::initBuffer(vertexBuffer, bufferCreateInfo);

					void* memoryLocation = logicalDevice.mapMemory(vertexBuffer.memory, 0, bufferCreateInfo.size);
					memcpy(memoryLocation, vertices.data(), bufferCreateInfo.size);

					logicalDevice.unmapMemory(vertexBuffer.memory);
				}

				Triangle::~Triangle()
				{
					logicalDevice.destroyBuffer(vertexBuffer.instance);
					logicalDevice.freeMemory(vertexBuffer.memory);
				}
			}
		}
	}
}