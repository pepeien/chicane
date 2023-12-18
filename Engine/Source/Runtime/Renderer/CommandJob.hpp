#pragma once

#include "Base.hpp"

namespace Engine
{
    namespace Command
    {
        namespace Job
        {
            void start(const vk::CommandBuffer& inCommandBuffer);
            void end(
                const vk::CommandBuffer& inCommandBuffer,
                const vk::Queue& inQueue,
                const std::string& inDescription = "Commad Buffer"
            );
        }
    }
}