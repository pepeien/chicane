#pragma once

#include "Base.hpp"

namespace Engine
{
	namespace Runtime
	{
		namespace Renderer
		{
			namespace Queue
			{
				struct FamilyIndices
                {
                public:
                    bool isComplete();

                public:
                    std::optional<uint32_t> graphicsFamily;
                    std::optional<uint32_t> presentFamily;
                };

                void findFamilyInidices(FamilyIndices& outFamilyIndices, vk::PhysicalDevice& inDevice, vk::SurfaceKHR& inSurface);

				void initGraphicsQueue(
					vk::Queue& outQueue,
					vk::PhysicalDevice& inPhysicalDevice,
					vk::Device& inLogicalDevice,
					vk::SurfaceKHR& inSurface
				);

				void initPresentQueue(
					vk::Queue& outQueue,
					vk::PhysicalDevice& inPhysicalDevice,
					vk::Device& inLogicalDevice,
					vk::SurfaceKHR& inSurface
				);
			}
		}
	}
}