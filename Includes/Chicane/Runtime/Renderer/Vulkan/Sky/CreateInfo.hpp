#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Box/Asset/Sky/Parsed.hpp"

#include "Chicane/Runtime.hpp"

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