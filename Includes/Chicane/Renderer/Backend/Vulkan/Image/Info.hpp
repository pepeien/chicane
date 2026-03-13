#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Core/Image/Info.hpp"

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanImageInfo
        {
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