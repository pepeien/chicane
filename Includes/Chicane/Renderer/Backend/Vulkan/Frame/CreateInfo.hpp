#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Core/String.hpp"

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanFrameCreateInfo
        {
        public:
            String                     id;
            vk::Device                 logicalDevice;
            vk::RenderPass             renderPass;
            vk::Extent2D               extent;
            std::vector<vk::ImageView> attachments;
        };
    }
}