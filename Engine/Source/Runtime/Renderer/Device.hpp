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
            }
        }
    }
}