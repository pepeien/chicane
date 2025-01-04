#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"
#include "Chicane/Renderer/Vulkan/Buffer/CreateInfo.hpp"
#include "Chicane/Renderer/Vulkan/Buffer/Instance.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Buffer
        {
            void init(
                Instance& outBuffer,
                const CreateInfo& inCreateInfo
            );
            void allocate(
                Instance& outBuffer,
                const CreateInfo& inCreateInfo
            );
            void copy(
                const Instance& inSourceBuffer,
                const Instance& inDestinationBuffer,
                const vk::DeviceSize& inAllocationSize,
                const vk::Queue& inQueue,
                const vk::CommandBuffer& inCommandBuffer
            );
            void destroy(const vk::Device& inLogicalDevice, const Instance& inBuffer);
        }
    }
}