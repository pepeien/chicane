#pragma once

#include "Chicane/Runtime/Renderer/Vulkan/Base.hpp"
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
}