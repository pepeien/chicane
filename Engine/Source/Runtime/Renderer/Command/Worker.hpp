#pragma once

#include "Base.hpp"

namespace Chicane
{
    namespace Command
    {
        namespace Worker
        {
            void startJob(const vk::CommandBuffer& inCommandBuffer);
            void endJob(
                const vk::CommandBuffer& inCommandBuffer,
                const vk::Queue& inQueue,
                const std::string& inDescription = "Commad Buffer"
            );
        }
    }
}