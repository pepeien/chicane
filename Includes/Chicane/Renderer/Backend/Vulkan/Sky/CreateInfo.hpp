#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Core/Image.hpp"

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanSkyCreateInfo
        {
        public:
            Image::List             images;

            vk::Device              logicalDevice;
            vk::PhysicalDevice      physicalDevice;
            vk::CommandBuffer       commandBuffer;
            vk::Queue               queue;
            vk::DescriptorSetLayout descriptorSetLayout;
            vk::DescriptorPool      descriptorPool;
        };
    }
}