#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"
#include "Chicane/Renderer/Vulkan/CommandBuffer/CreateInfo.hpp"
#include "Chicane/Renderer/Vulkan/CommandBuffer/Pool.hpp"
#include "Chicane/Renderer/Vulkan/CommandBuffer/Worker.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace CommandBuffer
        {
            void init(std::vector<vk::CommandBuffer>& outCommandBuffers, const CreateInfo& inCreateInfo);
            void init(vk::CommandBuffer& outCommandBuffer, const CreateInfo& inCreateInfo);
        }
    }
}