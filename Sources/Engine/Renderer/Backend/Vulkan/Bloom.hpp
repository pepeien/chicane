#pragma once

#include <array>

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"
#include "Backend/Vulkan/Image/Info.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class VulkanAllocator;

        struct CHICANE_RENDERER VulkanBloom
        {
        public:
            using Images       = std::array<VulkanImageInfo, 2>;
            using Framebuffers = std::array<vk::Framebuffer, 2>;

        public:
            void setup(
                vk::Device          inLogicalDevice,
                vk::PhysicalDevice  inPhysicalDevice,
                VulkanAllocator*    inAllocator,
                vk::Format          inFormat,
                const vk::Extent2D& inExtent
            );
            void destroy(vk::Device inLogicalDevice);
            void rebuildFramebuffers(
                vk::Device             inLogicalDevice,
                vk::RenderPass         inExtractPass,
                vk::RenderPass         inBlurPass,
                vk::RenderPass         inCompositePass,
                const VulkanImageInfo& inColorImage
            );

        private:
            static void destroyFramebuffer(vk::Device inLogicalDevice, vk::Framebuffer& inFramebuffer);
            static vk::Framebuffer makeFramebuffer(
                vk::Device inLogicalDevice, vk::RenderPass inPass, vk::ImageView inView, vk::Extent2D inExtent
            );

        public:
            Images          images               = {};
            Framebuffers    framebuffers         = {};
            vk::Framebuffer extractFramebuffer   = nullptr;
            vk::Framebuffer compositeFramebuffer = nullptr;
        };
    }
}
