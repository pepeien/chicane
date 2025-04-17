#pragma once

#include "Runtime/Renderer/Vulkan/Base.hpp"
#include "Runtime/Renderer/Vulkan/Buffer/CreateInfo.hpp"
#include "Runtime/Renderer/Vulkan/Buffer/Instance.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Buffer
        {
            CHICANE void init(
                Instance& outBuffer,
                const CreateInfo& inCreateInfo
            );
            CHICANE void allocate(
                Instance& outBuffer,
                const CreateInfo& inCreateInfo
            );
            CHICANE void copy(
                const Instance& inSourceBuffer,
                const Instance& inDestinationBuffer,
                const vk::DeviceSize& inAllocationSize,
                const vk::Queue& inQueue,
                const vk::CommandBuffer& inCommandBuffer
            );
            CHICANE void destroy(const vk::Device& inLogicalDevice, const Instance& inBuffer);
        }
    }
}