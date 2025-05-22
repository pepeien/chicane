#pragma once

#include "Chicane/Runtime/Renderer/Vulkan/Essential.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Sky
        {
            using Images = Box::Sky::ParsedSides;

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