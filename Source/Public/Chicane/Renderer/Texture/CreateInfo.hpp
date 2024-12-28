#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Texture
    {
        struct CreateInfo
        {
            std::vector<unsigned char> data;
            vk::Device                 logicalDevice;
            vk::PhysicalDevice         physicalDevice;
            vk::CommandBuffer          commandBuffer;
            vk::Queue                  queue;
        };
    }
}