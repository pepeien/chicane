#pragma once

#include "Base.hpp"

#include "Renderer/Frame/Instance.hpp"

namespace Engine
{
    namespace CommandBuffer
    {
        namespace Instance
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
}