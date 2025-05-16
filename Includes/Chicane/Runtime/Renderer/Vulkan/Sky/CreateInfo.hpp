#pragma once

#include "Chicane/Runtime/Renderer/Vulkan/Essential.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Sky
        {
            typedef Box::Sky::CompiledSides Images;

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