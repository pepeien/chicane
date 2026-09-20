#include "Backend/Vulkan/Allocator.hpp"

#include <algorithm>
#include <cstring>
#include <stdexcept>

#include <vk_mem_alloc.h>

#include "Backend/Vulkan/CommandBuffer/Worker.hpp"

namespace Chicane
{
    namespace Renderer
    {
        VulkanAllocator::VulkanAllocator()
            : m_allocator(nullptr),
              m_physicalDevice(nullptr),
              m_logicalDevice(nullptr),
              m_staging({}),
              m_stagingMapped(nullptr),
              m_stagingSize(0)
        {}

        std::size_t VulkanAllocator::queryDedicatedHeapSize(const vk::PhysicalDevice& inPhysicalDevice)
        {
            const vk::PhysicalDeviceMemoryProperties properties = inPhysicalDevice.getMemoryProperties();

            std::size_t dedicated   = 0;
            std::size_t deviceLocal = 0;

            for (std::uint32_t heapIndex = 0; heapIndex < properties.memoryHeapCount; heapIndex++)
            {
                const vk::MemoryHeap& heap = properties.memoryHeaps[heapIndex];
                if (!(heap.flags & vk::MemoryHeapFlagBits::eDeviceLocal))
                {
                    continue;
                }

                deviceLocal = std::max(deviceLocal, static_cast<std::size_t>(heap.size));

                bool bHasDeviceOnly = false;
                for (std::uint32_t typeIndex = 0; typeIndex < properties.memoryTypeCount; typeIndex++)
                {
                    const vk::MemoryType& type = properties.memoryTypes[typeIndex];
                    if (type.heapIndex != heapIndex)
                    {
                        continue;
                    }

                    const vk::MemoryPropertyFlags flags = type.propertyFlags;
                    if ((flags & vk::MemoryPropertyFlagBits::eDeviceLocal) &&
                        !(flags & vk::MemoryPropertyFlagBits::eHostVisible))
                    {
                        bHasDeviceOnly = true;

                        break;
                    }
                }

                if (bHasDeviceOnly)
                {
                    dedicated = std::max(dedicated, static_cast<std::size_t>(heap.size));
                }
            }

            return dedicated > 0 ? dedicated : deviceLocal;
        }

        void VulkanAllocator::init(
            const vk::Instance&       inInstance,
            const vk::PhysicalDevice& inPhysicalDevice,
            const vk::Device&         inLogicalDevice
        )
        {
            destroy();

            VmaAllocatorCreateInfo createInfo = {};
            createInfo.instance               = static_cast<VkInstance>(inInstance);
            createInfo.physicalDevice         = static_cast<VkPhysicalDevice>(inPhysicalDevice);
            createInfo.device                 = static_cast<VkDevice>(inLogicalDevice);
            createInfo.vulkanApiVersion =
                VK_MAKE_API_VERSION(0, VULKAN_MAJOR_VERSION, VULKAN_MINOR_VERSION, VULKAN_PATCH_VERSION);

            const VkResult result = vmaCreateAllocator(&createInfo, &m_allocator);
            if (result != VK_SUCCESS || !m_allocator)
            {
                throw std::runtime_error("Failed to create the Vulkan memory allocator");
            }

            m_physicalDevice = inPhysicalDevice;
            m_logicalDevice  = inLogicalDevice;
        }

        void VulkanAllocator::destroy()
        {
            destroyBuffer(m_staging);
            m_stagingMapped = nullptr;
            m_stagingSize   = 0;

            if (m_allocator)
            {
                vmaDestroyAllocator(m_allocator);
                m_allocator = nullptr;
            }

            m_physicalDevice = nullptr;
            m_logicalDevice  = nullptr;
        }

        bool VulkanAllocator::isValid() const
        {
            return m_allocator != nullptr;
        }

        VmaAllocator_T* VulkanAllocator::handle() const
        {
            return m_allocator;
        }

        void VulkanAllocator::createBuffer(VulkanBuffer& outBuffer, const VulkanBufferCreateInfo& inCreateInfo)
        {
            destroyBuffer(outBuffer);

            VkBufferCreateInfo bufferInfo = {};
            bufferInfo.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size               = inCreateInfo.size;
            bufferInfo.usage              = static_cast<VkBufferUsageFlags>(inCreateInfo.usage);
            bufferInfo.sharingMode        = VK_SHARING_MODE_EXCLUSIVE;

            VmaAllocationCreateInfo allocationInfo = {};
            const bool              bHostVisible =
                static_cast<bool>(inCreateInfo.memoryProperties & vk::MemoryPropertyFlagBits::eHostVisible);
            if (bHostVisible)
            {
                allocationInfo.usage = VMA_MEMORY_USAGE_AUTO;
                allocationInfo.flags =
                    VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
            }
            else
            {
                allocationInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
            }

            VkBuffer          buffer     = VK_NULL_HANDLE;
            VmaAllocation     allocation = VK_NULL_HANDLE;
            VmaAllocationInfo info       = {};
            const VkResult    result =
                vmaCreateBuffer(m_allocator, &bufferInfo, &allocationInfo, &buffer, &allocation, &info);
            if (result != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create a Vulkan buffer");
            }

            outBuffer.instance   = buffer;
            outBuffer.memory     = info.deviceMemory;
            outBuffer.mapped     = info.pMappedData;
            outBuffer.owner      = this;
            outBuffer.vma        = m_allocator;
            outBuffer.allocation = allocation;
            outBuffer.size       = inCreateInfo.size;
            outBuffer.usage      = inCreateInfo.usage;
            outBuffer.properties = inCreateInfo.memoryProperties;
        }

        void VulkanAllocator::destroyBuffer(VulkanBuffer& inBuffer)
        {
            if (!inBuffer.allocation || !inBuffer.vma)
            {
                inBuffer = {};

                return;
            }

            vmaDestroyBuffer(inBuffer.vma, static_cast<VkBuffer>(inBuffer.instance), inBuffer.allocation);
            inBuffer = {};
        }

        void VulkanAllocator::createImage(
            VulkanImageInfo&                   outImage,
            const VulkanImageCreateInfo&       inCreateInfo,
            const VulkanImageMemoryCreateInfo& inMemoryCreateInfo
        )
        {
            destroyImage(outImage);

            vk::ImageCreateInfo imageCreateInfo;
            imageCreateInfo.flags         = vk::ImageCreateFlagBits() | inCreateInfo.flags;
            imageCreateInfo.imageType     = vk::ImageType::e2D;
            imageCreateInfo.extent        = vk::Extent3D(inCreateInfo.width, inCreateInfo.height, 1);
            imageCreateInfo.mipLevels     = std::max(1u, inCreateInfo.mipLevels);
            imageCreateInfo.arrayLayers   = inCreateInfo.count;
            imageCreateInfo.format        = inCreateInfo.format;
            imageCreateInfo.samples       = vk::SampleCountFlagBits::e1;
            imageCreateInfo.tiling        = inCreateInfo.tiling;
            imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
            imageCreateInfo.usage         = inCreateInfo.usage;
            imageCreateInfo.sharingMode   = vk::SharingMode::eExclusive;

            const VkImageCreateInfo vkImageInfo = imageCreateInfo;

            VmaAllocationCreateInfo allocationInfo = {};
            const bool              bHostVisible =
                static_cast<bool>(inMemoryCreateInfo.properties & vk::MemoryPropertyFlagBits::eHostVisible);
            if (bHostVisible)
            {
                allocationInfo.usage = VMA_MEMORY_USAGE_AUTO;
                allocationInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
            }
            else
            {
                allocationInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
            }

            VkImage           image      = VK_NULL_HANDLE;
            VmaAllocation     allocation = VK_NULL_HANDLE;
            VmaAllocationInfo info       = {};
            const VkResult    result =
                vmaCreateImage(m_allocator, &vkImageInfo, &allocationInfo, &image, &allocation, &info);
            if (result != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create a Vulkan image");
            }

            outImage.instance   = image;
            outImage.memory     = info.deviceMemory;
            outImage.allocator  = m_allocator;
            outImage.allocation = allocation;
        }

        void VulkanAllocator::destroyImage(VulkanImageInfo& inImage)
        {
            if (inImage.allocation && inImage.allocator)
            {
                vmaDestroyImage(inImage.allocator, static_cast<VkImage>(inImage.instance), inImage.allocation);
            }

            inImage.instance   = nullptr;
            inImage.memory     = nullptr;
            inImage.allocator  = nullptr;
            inImage.allocation = nullptr;
        }

        void* VulkanAllocator::map(VulkanBuffer& inBuffer)
        {
            if (inBuffer.mapped)
            {
                return inBuffer.mapped;
            }

            if (!inBuffer.vma || !inBuffer.allocation)
            {
                return nullptr;
            }

            void* mapped = nullptr;
            vmaMapMemory(inBuffer.vma, inBuffer.allocation, &mapped);
            inBuffer.mapped = mapped;

            return mapped;
        }

        void VulkanAllocator::unmap(VulkanBuffer& inBuffer)
        {
            if (!inBuffer.mapped || !inBuffer.vma || !inBuffer.allocation)
            {
                return;
            }

            vmaUnmapMemory(inBuffer.vma, inBuffer.allocation);
            inBuffer.mapped = nullptr;
        }

        void VulkanAllocator::ensureStaging(vk::DeviceSize inSize)
        {
            if (m_staging.instance && m_stagingSize >= inSize)
            {
                return;
            }

            const vk::DeviceSize size = std::max(inSize, static_cast<vk::DeviceSize>(RESOURCE_STAGING_INITIAL_BYTES));

            VulkanBufferCreateInfo createInfo;
            createInfo.size           = size;
            createInfo.usage          = vk::BufferUsageFlagBits::eTransferSrc;
            createInfo.logicalDevice  = m_logicalDevice;
            createInfo.physicalDevice = m_physicalDevice;
            createInfo.memoryProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
            createInfo.allocator = this;

            createBuffer(m_staging, createInfo);
            m_stagingMapped = m_staging.mapped;
            m_stagingSize   = size;
        }

        void VulkanAllocator::upload(
            const vk::Buffer&        inDestination,
            vk::DeviceSize           inOffset,
            const void*              inData,
            vk::DeviceSize           inSize,
            const vk::CommandBuffer& inCommandBuffer,
            const vk::Queue&         inQueue
        )
        {
            if (!inData || inSize == 0)
            {
                return;
            }

            ensureStaging(inSize);
            std::memcpy(m_stagingMapped, inData, static_cast<std::size_t>(inSize));

            VulkanCommandBufferWorker::startJob(inCommandBuffer);

            vk::BufferCopy copy;
            copy.srcOffset = 0;
            copy.dstOffset = inOffset;
            copy.size      = inSize;
            inCommandBuffer.copyBuffer(m_staging.instance, inDestination, 1, &copy);

            VulkanCommandBufferWorker::endJob(inCommandBuffer, inQueue, "VMA staging upload");
        }
    }
}
