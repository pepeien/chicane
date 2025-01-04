#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace CommandBuffer
        {
            namespace Worker
            {
                void startJob(const vk::CommandBuffer& inCommandBuffer);
                void endJob(
                    const vk::CommandBuffer& inCommandBuffer,
                    const vk::Queue& inQueue,
                    const std::string& inDescription = "Command Buffer"
                );
                void endJob(
                    const vk::Queue& inQueue,
                    const vk::SubmitInfo& inSubmitInfo,
                    const std::string& inDescription = "Command Buffer"
                );
            }
        }
    }
}