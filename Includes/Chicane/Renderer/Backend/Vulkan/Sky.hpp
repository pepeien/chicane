#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Buffer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Bundle.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image/Data.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Sky/CreateInfo.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER VulkanSky
        {
        public:
            VulkanSky(const VulkanSkyCreateInfo& inCreateInfo);
            ~VulkanSky();

        public:
            void bind(const vk::CommandBuffer& inCommandBuffer, const vk::PipelineLayout& inPipelineLayout);

        private:
            void initImage();
            void copyPixels();
            void initDescriptorSet();

        private:
            VulkanImageData        m_image;
            Image::List            m_images;

            VulkanDescriptorBundle m_descriptor;

            vk::Device             m_logicalDevice;
            vk::PhysicalDevice     m_physicalDevice;
            vk::CommandBuffer      m_commandBuffer;
            vk::Queue              m_queue;
        };
    }
}