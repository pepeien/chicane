#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Image.hpp"
#include "Chicane/Box/Asset/CubeMap.hpp"
#include "Chicane/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace CubeMap
        {
            typedef std::unordered_map<Box::CubeMap::Side, Chicane::Image::Data> Images;

            struct CreateInfo
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