#pragma once

#include "Chicane/Base.hpp"

#include "Chicane/Core/Log.hpp"

namespace Chicane
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