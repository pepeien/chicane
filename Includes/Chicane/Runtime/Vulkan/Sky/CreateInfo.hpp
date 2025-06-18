#pragma once

#include "Chicane/Box/Asset/Sky/Types.hpp"
#include "Chicane/Runtime/Vulkan/Essential.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Sky
        {
            using Images = Box::SkyParsedSides;

            struct CHICANE_RUNTIME CreateInfo
            {
            public:
                Images                  images;

                vk::Device              logicalDevice;
                vk::PhysicalDevice      physicalDevice;
                vk::CommandBuffer       commandBuffer;
                vk::Queue               queue;
                vk::DescriptorSetLayout descriptorSetLayout;
                vk::DescriptorPool      descriptorPool;
            };
        }
    }
}