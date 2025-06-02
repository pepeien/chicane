#pragma once

#include "Chicane/Runtime/Vulkan/Essential.hpp"
#include "Chicane/Runtime/Vulkan/Buffer/CreateInfo.hpp"
#include "Chicane/Runtime/Vulkan/Buffer/Instance.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Buffer
        {
            CHICANE_RUNTIME void init(
                Instance& outBuffer,
                const CreateInfo& inCreateInfo
            );
            CHICANE_RUNTIME void allocate(
                Instance& outBuffer,
                const CreateInfo& inCreateInfo
            );
            CHICANE_RUNTIME void copy(
                const Instance& inSourceBuffer,
                const Instance& inDestinationBuffer,
                const vk::DeviceSize& inAllocationSize,
                const vk::Queue& inQueue,
                const vk::CommandBuffer& inCommandBuffer
            );
            CHICANE_RUNTIME void destroy(const vk::Device& inLogicalDevice, Instance& inBuffer);
        }
    }
}