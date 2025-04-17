#pragma once

#include "Runtime/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace CommandBuffer
        {
            namespace Worker
            {
                CHICANE void startJob(const vk::CommandBuffer& inCommandBuffer);
                CHICANE void endJob(
                    const vk::CommandBuffer& inCommandBuffer,
                    const vk::Queue& inQueue,
                    const std::string& inDescription = "Command Buffer"
                );
                CHICANE void endJob(
                    const vk::Queue& inQueue,
                    const vk::SubmitInfo& inSubmitInfo,
                    const std::string& inDescription = "Command Buffer"
                );
            }
        }
    }
}