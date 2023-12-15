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

                void findFamilyInidices(
                    FamilyIndices& outFamilyIndices,
                    const vk::PhysicalDevice& inPhysicalDevice,
                    const vk::SurfaceKHR& inSurface
                );

                void initGraphicsQueue(
                    vk::Queue& outQueue,
                    const vk::PhysicalDevice& inPhysicalDevice,
                    const vk::Device& inLogicalDevice,
                    const vk::SurfaceKHR& inSurface
                );

                void initPresentQueue(
                    vk::Queue& outQueue,
                    const vk::PhysicalDevice& inPhysicalDevice,
                    const vk::Device& inLogicalDevice,
                    const vk::SurfaceKHR& inSurface
                );
            }
        }
    }
}