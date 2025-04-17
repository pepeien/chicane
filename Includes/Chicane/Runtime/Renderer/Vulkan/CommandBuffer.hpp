#pragma once

#include "Runtime/Renderer/Vulkan/Base.hpp"
#include "Runtime/Renderer/Vulkan/CommandBuffer/CreateInfo.hpp"
#include "Runtime/Renderer/Vulkan/CommandBuffer/Pool.hpp"
#include "Runtime/Renderer/Vulkan/CommandBuffer/Worker.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace CommandBuffer
        {
            CHICANE void init(std::vector<vk::CommandBuffer>& outCommandBuffers, const CreateInfo& inCreateInfo);
            CHICANE void init(vk::CommandBuffer& outCommandBuffer, const CreateInfo& inCreateInfo);
        }
    }
}