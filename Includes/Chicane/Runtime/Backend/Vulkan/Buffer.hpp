#pragma once

#include "Chicane/Runtime/Backend/Vulkan/Buffer/CreateInfo.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Essential.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        struct CHICANE_RUNTIME Buffer
        {
        public:
            Buffer();

        public:
            void init(const BufferCreateInfo& inCreateInfo);
            void allocate(const BufferCreateInfo& inCreateInfo);
            void copy(
                const Buffer& inDestination,
                const vk::DeviceSize& inAllocationSize,
                const vk::Queue& inQueue,
                const vk::CommandBuffer& inCommandBuffer
            );
            void destroy(const vk::Device& inLogicalDevice);

        public:
            vk::Buffer       instance;
            vk::DeviceMemory memory;
        };
    }
}