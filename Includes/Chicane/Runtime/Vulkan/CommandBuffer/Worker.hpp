#pragma once

#include "Chicane/Runtime/Vulkan/Essential.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace CommandBuffer
        {
            namespace Worker
            {
                CHICANE_RUNTIME void startJob(const vk::CommandBuffer& inCommandBuffer);
                CHICANE_RUNTIME void endJob(
                    const vk::CommandBuffer& inCommandBuffer,
                    const vk::Queue& inQueue,
                    const std::string& inDescription = "Command Buffer"
                );
                CHICANE_RUNTIME void endJob(
                    const vk::Queue& inQueue,
                    const vk::SubmitInfo& inSubmitInfo,
                    const std::string& inDescription = "Command Buffer"
                );
            }
        }
    }
}