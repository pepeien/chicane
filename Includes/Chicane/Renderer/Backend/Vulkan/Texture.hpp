#pragma once

#include <cstdint>

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image/Info.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Texture/CreateInfo.hpp"
#include "Chicane/Renderer/Draw/Texture.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER VulkanTexture : public VulkanImageInfo
        {
        public:
            VulkanTexture(const VulkanTextureCreateInfo& inCreateInfo);
            ~VulkanTexture();

        public:
            bool matches(const DrawTexture& inTexture) const;

        private:
            void initExtent(const VulkanTextureCreateInfo& inCreateInfo);
            void initInstance();
            void initSampler();
            void initMemory();
            void initView();
            void copyPixels(const VulkanTextureCreateInfo& inCreateInfo);

        private:
            vk::Device         m_logicalDevice;
            vk::PhysicalDevice m_physicalDevice;
            vk::CommandBuffer  m_commandBuffer;
            vk::Queue          m_queue;

            std::uint32_t      m_sourceWidth;
            std::uint32_t      m_sourceHeight;
            std::uint32_t      m_residentMinMip;
            std::uint32_t      m_mipLevels;
        };
    }
}
