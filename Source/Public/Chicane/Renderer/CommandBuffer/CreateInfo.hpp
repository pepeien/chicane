#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
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