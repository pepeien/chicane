#pragma once

#include "Chicane/Runtime/Backend/Vulkan/Essential.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Texture
        {
            struct CHICANE_RUNTIME CreateInfo
            {
            public:
                Chicane::Image           image;

                vk::Device               logicalDevice;
                vk::PhysicalDevice       physicalDevice;
                vk::CommandBuffer        commandBuffer;
                vk::Queue                queue;
            };
        }
    }
}