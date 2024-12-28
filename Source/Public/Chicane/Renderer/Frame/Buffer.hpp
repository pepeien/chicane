#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Renderer/CommandBuffer/CreateInfo.hpp"
#include "Chicane/Renderer/Frame/Buffer/CreateInfo.hpp"
#include "Chicane/Renderer/Frame/Instance.hpp"

namespace Chicane
{
    namespace Frame
    {
        namespace Buffer
        {
            void init(
                Instance& outFrame,
                const CreateInfo& inCreateInfo
            );
            void initCommand(
                std::vector<Instance>& outFrames,
                const CommandBuffer::CreateInfo& inCreateInfo
            );
            void initCommand(
                Instance& outFrame,
                const CommandBuffer::CreateInfo& inCreateInfo
            );
        }
    }
}