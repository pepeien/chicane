#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Texture
        {
            struct CreateInfo
            {
            public:
                std::vector<unsigned char> data;
                vk::Device                 logicalDevice;
                vk::PhysicalDevice         physicalDevice;
                vk::CommandBuffer          commandBuffer;
                vk::Queue                  queue;
            };
        }
    }
}