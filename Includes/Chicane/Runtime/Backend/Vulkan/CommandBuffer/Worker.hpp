#pragma once

#include "Chicane/Runtime/Backend/Vulkan/Essential.hpp"

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
                    const String& inDescription = "Command Buffer"
                );
                CHICANE_RUNTIME void endJob(
                    const vk::Queue& inQueue,
                    const vk::SubmitInfo& inSubmitInfo,
                    const String& inDescription = "Command Buffer"
                );
            }
        }
    }
}