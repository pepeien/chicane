#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Core/View.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Frame/Buffer/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Frame/Resource.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image/Data.hpp"
#include "Chicane/Renderer/Draw.hpp"
#include "Chicane/Renderer/Draw/Poly/2D/Instance.hpp"
#include "Chicane/Renderer/Draw/Poly/3D/Instance.hpp"
#include "Chicane/Renderer/Frame.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER VulkanSwapchainImage
        {
        public:
            // Lifecycle
            void destroy();

            // Buffer
            void addBuffer(const VulkanFrameBufferCreateInfo& inCreateInfo);

            // Sychronization
            void setupSync();
            void destroySync();

            // Image
            void setupColorImage(vk::Format inFormat, const vk::Extent2D& inExtent);
            void destroyColorImage();

            void setupDepthImage(vk::Format inFormat, const vk::Extent2D& inExtent);
            void destroyDepthImage();

            // Frambuffer
            void addFramebuffer(const String& inId, const vk::Framebuffer& inFramebuffer);
            vk::Framebuffer getFramebuffer(const String& inId) const;

        public:
            // Device
            vk::Device                                  logicalDevice;
            vk::PhysicalDevice                          physicalDevice;

            // Image
            VulkanImageData                             colorImage;
            VulkanImageData                             depthImage;

            // Buffer
            std::unordered_map<String, vk::Framebuffer> framebuffers;

            // Sychronization
            vk::Semaphore                               renderFineshedSemaphore;
        };
    }
}