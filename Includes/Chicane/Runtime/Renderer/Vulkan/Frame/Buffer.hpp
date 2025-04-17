#pragma once

#include "Runtime/Renderer/Vulkan/Base.hpp"
#include "Runtime/Renderer/Vulkan/CommandBuffer/CreateInfo.hpp"
#include "Runtime/Renderer/Vulkan/Frame/Buffer/CreateInfo.hpp"
#include "Runtime/Renderer/Vulkan/Frame/Instance.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Frame
        {
            namespace Buffer
            {
                CHICANE void init(
                    Instance& outFrame,
                    const CreateInfo& inCreateInfo
                );
                CHICANE void initCommand(
                    std::vector<Instance>& outFrames,
                    const CommandBuffer::CreateInfo& inCreateInfo
                );
                CHICANE void initCommand(
                    Instance& outFrame,
                    const CommandBuffer::CreateInfo& inCreateInfo
                );
            }
        }
    }
}