#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Renderer/CommandBuffer/CreateInfo.hpp"

namespace Chicane
{
    namespace CommandBuffer
    {
        void init(std::vector<vk::CommandBuffer>& outCommandBuffers, const CreateInfo& inCreateInfo);
        void init(vk::CommandBuffer& outCommandBuffer, const CreateInfo& inCreateInfo);
    }
}