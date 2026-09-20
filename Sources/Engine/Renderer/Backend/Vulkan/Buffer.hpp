#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"
#include "Backend/Vulkan/Buffer/CreateInfo.hpp"

struct VmaAllocator_T;
struct VmaAllocation_T;

namespace Chicane
{
    namespace Renderer
    {
        class VulkanAllocator;

        struct CHICANE_RENDERER VulkanBuffer
        {
        public:
            VulkanBuffer();

        public:
            void init(const VulkanBufferCreateInfo& inCreateInfo);
            void destroy();
            void* map();
            void unmap();
            void copy(
                const VulkanBuffer&      inDestination,
                const vk::DeviceSize&    inAllocationSize,
                const vk::Queue&         inQueue,
                const vk::CommandBuffer& inCommandBuffer
            );

        public:
            vk::Buffer              instance    = nullptr;
            vk::DeviceMemory        memory      = nullptr;
            void*                   mapped      = nullptr;
            VulkanAllocator*        owner       = nullptr;
            VmaAllocator_T*         vma         = nullptr;
            VmaAllocation_T*        allocation  = nullptr;
            vk::DeviceSize          size        = 0;
            vk::BufferUsageFlags    usage       = {};
            vk::MemoryPropertyFlags properties  = {};
        };
    }
}
