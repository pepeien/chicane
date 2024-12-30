#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace CubeMap
    {
        typedef std::vector<std::vector<unsigned char>> Data;

        struct CreateInfo
        {
        public:
            Data                    data;
            vk::Device              logicalDevice;
            vk::PhysicalDevice      physicalDevice;
            vk::CommandBuffer       commandBuffer;
            vk::Queue               queue;
            vk::DescriptorSetLayout descriptorSetLayout;
            vk::DescriptorPool      descriptorPool;
        };
    }
}