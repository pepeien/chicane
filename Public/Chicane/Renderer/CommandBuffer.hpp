#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Renderer/Frame.hpp"

namespace Chicane
{
    namespace CommandBuffer
    {
        struct CreateInfo
        {
            vk::Device logicalDevice;
            vk::CommandPool commandPool;
        };

        void init(std::vector<vk::CommandBuffer>& outCommandBuffers, const CreateInfo& inCreateInfo);
        void init(vk::CommandBuffer& outCommandBuffer, const CreateInfo& inCreateInfo);
    }
}