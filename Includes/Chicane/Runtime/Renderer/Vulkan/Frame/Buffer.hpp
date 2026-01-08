#pragma once

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/CommandBuffer/CreateInfo.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Frame/Buffer/CreateInfo.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Frame/Instance.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Frame
        {
            namespace Buffer
            {
                CHICANE_RUNTIME void init(Instance& outFrame, const CreateInfo& inCreateInfo);
                CHICANE_RUNTIME void initCommand(std::vector<Instance>& outFrames, const CommandBuffer::CreateInfo& inCreateInfo);
                CHICANE_RUNTIME void initCommand(Instance& outFrame, const CommandBuffer::CreateInfo& inCreateInfo);
            }
        }
    }
}