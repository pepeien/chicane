#pragma once

#include "Chicane/Runtime/Backend/Vulkan/Essential.hpp"
#include "Chicane/Runtime/Backend/Vulkan/CommandBuffer/CreateInfo.hpp"
#include "Chicane/Runtime/Backend/Vulkan/CommandBuffer/Pool.hpp"
#include "Chicane/Runtime/Backend/Vulkan/CommandBuffer/Worker.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace CommandBuffer
        {
            CHICANE_RUNTIME void init(std::vector<vk::CommandBuffer>& outCommandBuffers, const CreateInfo& inCreateInfo);
            CHICANE_RUNTIME void init(vk::CommandBuffer& outCommandBuffer, const CreateInfo& inCreateInfo);
        }
    }
}