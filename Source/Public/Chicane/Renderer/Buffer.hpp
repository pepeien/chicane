#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Renderer/Buffer/CreateInfo.hpp"
#include "Chicane/Renderer/Buffer/Instance.hpp"

namespace Chicane
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