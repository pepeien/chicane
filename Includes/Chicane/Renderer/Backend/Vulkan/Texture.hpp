#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image/Info.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Texture/CreateInfo.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER VulkanTexture : public VulkanImageInfo
        {
        public:
            VulkanTexture(const VulkanTextureCreateInfo& inCreateInfo);
            ~VulkanTexture();

        private:
            void initExtent(const Image::Reference inImage);
            void initInstance();
            void initSampler();
            void initMemory();
            void initView();
            void copyPixels(const Image::Reference inImage);

        private:
            vk::Device         m_logicalDevice;
            vk::PhysicalDevice m_physicalDevice;
            vk::CommandBuffer  m_commandBuffer;
            vk::Queue          m_queue;
        };
    }
}