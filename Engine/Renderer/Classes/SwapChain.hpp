#pragma once

#include "Base.hpp"

namespace Engine
{
    namespace Core
    {
        namespace Render
        {
            namespace SwapChain
            {
                struct SupportDetails
                {
                public:
                    vk::SurfaceCapabilitiesKHR capabilities;
                    std::vector<vk::SurfaceFormatKHR> formats;
                    std::vector<vk::PresentModeKHR> presentModes;
                };

                struct Frame
                {
                    vk::Image image;
                    vk::ImageView imageView;
                    vk::Framebuffer framebuffer;
                    vk::CommandBuffer commandBuffer;
                };

                struct Bundle
                {
                public:
                    vk::SwapchainKHR swapchain;
                    std::vector<Frame> frames;
                    vk::Format format;
                    vk::Extent2D extent;
                };

                void querySupport(SupportDetails& allocator, vk::PhysicalDevice& inPhyisicalDevice, vk::SurfaceKHR& inSurface);

                void pickSurfaceFormat(vk::SurfaceFormatKHR& allocator, std::vector<vk::SurfaceFormatKHR>& inSurfaceFormats);
                void pickPresentMode(vk::PresentModeKHR& allocator, std::vector<vk::PresentModeKHR>& inPresentModes);
                void pickExtent(vk::Extent2D& allocator, uint32_t inWidth, uint32_t inHeight, vk::SurfaceCapabilitiesKHR& inCapabilities);
            }
        }
    }
}