#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Core/Image.hpp"

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class VulkanAllocator;

        struct CHICANE_RENDERER VulkanSkyCreateInfo
        {
        public:
            Image::References       images;

            vk::Device              logicalDevice;
            vk::PhysicalDevice      physicalDevice;
            vk::CommandBuffer       commandBuffer;
            vk::Queue               queue;
            VulkanAllocator*        allocator = nullptr;
            vk::DescriptorSetLayout descriptorSetLayout;
            vk::DescriptorPool      descriptorPool;
        };
    }
}