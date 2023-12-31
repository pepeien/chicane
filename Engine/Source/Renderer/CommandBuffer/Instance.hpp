#pragma once

#include "Base.hpp"

#include "Renderer/Frame/Instance.hpp"

namespace Chicane
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

            void init(vk::CommandBuffer& outCommandBuffer, const CreateInfo& inCreateInfo);
        }
    }
}