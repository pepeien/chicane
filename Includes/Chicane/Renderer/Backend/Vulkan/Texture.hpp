#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image/Data.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Texture/CreateInfo.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER VulkanTexture
        {
        public:
            VulkanTexture(const VulkanTextureCreateInfo& inCreateInfo);
            ~VulkanTexture();

        public:
            const VulkanImageData& getImage() const;

        private:
            void populate();

        private:
            VulkanImageData    m_image;

            vk::Device         m_logicalDevice;
            vk::PhysicalDevice m_physicalDevice;
            vk::CommandBuffer  m_commandBuffer;
            vk::Queue          m_queue;
        };
    }
}