#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Buffer/CreateInfo.hpp"

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