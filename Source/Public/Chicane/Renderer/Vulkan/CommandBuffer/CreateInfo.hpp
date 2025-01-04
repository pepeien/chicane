#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace CommandBuffer
        {
            struct CreateInfo
            {
            public:
                vk::Device      logicalDevice;
                vk::CommandPool commandPool;
            };
        }
    }
}