#pragma once

#include "Base.hpp"

#include "Core/Log.hpp"

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
        }
    }
}