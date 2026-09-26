#pragma once

#include <cstddef>
#include <cstdint>

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"

#include "Backend/Vulkan/Buffer.hpp"
#include "Backend/Vulkan/Buffer/CreateInfo.hpp"
#include "Backend/Vulkan/Image/CreateInfo.hpp"
#include "Backend/Vulkan/Image/Info.hpp"
#include "Backend/Vulkan/Image/Memory/CreateInfo.hpp"

struct VmaAllocator_T;
struct VmaAllocation_T;

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER VulkanAllocator
        {
        public:
            static std::size_t sQueryDedicatedHeapSize(const vk::PhysicalDevice& inPhysicalDevice);

        public:
            static void sDestroyImage(VulkanImageInfo& inImage);

        public:
            VulkanAllocator();

        public:
            void init(
                const vk::Instance&       inInstance,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::Device&         inLogicalDevice
            );
            void destroy();

            bool isValid() const;
            VmaAllocator_T* handle() const;

            void createBuffer(VulkanBuffer& outBuffer, const VulkanBufferCreateInfo& inCreateInfo);
            void destroyBuffer(VulkanBuffer& inBuffer);

            void createImage(
                VulkanImageInfo&                   outImage,
                const VulkanImageCreateInfo&       inCreateInfo,
                const VulkanImageMemoryCreateInfo& inMemoryCreateInfo
            );

            void* map(VulkanBuffer& inBuffer);
            void unmap(VulkanBuffer& inBuffer);

            void upload(
                const vk::Buffer&        inDestination,
                vk::DeviceSize           inOffset,
                const void*              inData,
                vk::DeviceSize           inSize,
                const vk::CommandBuffer& inCommandBuffer,
                const vk::Queue&         inQueue
            );

        private:
            void ensureStaging(vk::DeviceSize inSize);

        private:
            VmaAllocator_T*    m_allocator;
            vk::PhysicalDevice m_physicalDevice;
            vk::Device         m_logicalDevice;
            VulkanBuffer       m_staging;
            void*              m_stagingMapped;
            vk::DeviceSize     m_stagingSize;
        };
    }
}
