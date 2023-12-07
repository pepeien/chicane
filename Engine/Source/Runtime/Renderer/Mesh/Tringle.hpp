#pragma once

#include "Base.hpp"

#include "Renderer/Device.hpp"

namespace Engine
{
	namespace Runtime
	{
		namespace Renderer
		{
			namespace Mesh
			{
				class Triangle
				{
				public:
					Triangle(vk::Device& inLogicalDevice, vk::PhysicalDevice& inPhysicalDevice);
					~Triangle();

				public:
					Device::Buffer vertexBuffer;

				private:
					vk::Device logicalDevice;
				};
			}
		}
	}
}