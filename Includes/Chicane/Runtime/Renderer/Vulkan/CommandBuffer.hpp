#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/CommandBuffer/CreateInfo.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace CommandBuffer
        {
            CHICANE_RUNTIME void init(
                std::vector<vk::CommandBuffer>& outCommandBuffers, const CreateInfo& inCreateInfo
            );
            CHICANE_RUNTIME void init(vk::CommandBuffer& outCommandBuffer, const CreateInfo& inCreateInfo);
        }
    }
}