#pragma once

#include "Chicane/Renderer.hpp"
#include "Backend/Vulkan/Descriptor/Bundle.hpp"
#include "Backend/Vulkan/Image/Info.hpp"
#include "Backend/Vulkan/Sky/CreateInfo.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class VulkanAllocator;

        class CHICANE_RENDERER VulkanSky : public VulkanImageInfo
        {
        public:
            VulkanSky(const VulkanSkyCreateInfo& inCreateInfo);
            ~VulkanSky();

        public:
            void bind(const vk::CommandBuffer& inCommandBuffer, const vk::PipelineLayout& inPipelineLayout);

            vk::DescriptorImageInfo getDescriptorImageInfo() const;

        private:
            void initExtent(const Image::References& inImages);
            void initMipLevels();
            void initInstance(std::uint32_t inCount);
            void initSampler();
            void initView(std::uint32_t inCount);
            void copyPixels(const Image::References& inImages);
            void initDescriptorSet();

        private:
            vk::Device             m_logicalDevice;
            vk::PhysicalDevice     m_physicalDevice;
            vk::CommandBuffer      m_commandBuffer;
            vk::Queue              m_queue;
            VulkanAllocator*       m_allocator;
            std::uint32_t          m_mipLevels;
            VulkanDescriptorBundle m_descriptor;
        };
    }
}