#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Core/Image.hpp"

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanImageData : public Image
        {
        public:
            inline VulkanImageData(const Image& inImage)
                : Image()
            {
                m_width   = inImage.getWidth();
                m_height  = inImage.getWidth();
                m_channel = inImage.getChannel();
                m_format  = inImage.getFormat();
                m_pixels  = inImage.getPixels();
            }

            inline VulkanImageData()
                : Image()
            {}

            inline ~VulkanImageData() = default;

        public:
            vk::Image        instance;
            vk::ImageView    view;
            vk::DeviceMemory memory;
            vk::Sampler      sampler;
            vk::Format       format;
            vk::Extent2D     extent;
        };
    }
}