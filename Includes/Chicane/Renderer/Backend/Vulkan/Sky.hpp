#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Bundle.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image/Info.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Sky/CreateInfo.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER VulkanSky : public VulkanImageInfo
        {
        public:
            VulkanSky(const VulkanSkyCreateInfo& inCreateInfo);
            ~VulkanSky();

        public:
            void bind(const vk::CommandBuffer& inCommandBuffer, const vk::PipelineLayout& inPipelineLayout);

        private:
            void initExtent(const Image::References& inImages);
            void initInstance(std::uint32_t inCount);
            void initSampler();
            void initMemory();
            void initView(std::uint32_t inCount);
            void copyPixels(const Image::References& inImages);
            void initDescriptorSet();

        private:
            vk::Device             m_logicalDevice;
            vk::PhysicalDevice     m_physicalDevice;
            vk::CommandBuffer      m_commandBuffer;
            vk::Queue              m_queue;
            VulkanDescriptorBundle m_descriptor;
        };
    }
}