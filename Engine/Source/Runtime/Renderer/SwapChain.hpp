#pragma once

#include "Base.hpp"

#include "Queue.hpp"

namespace Engine
{
	namespace Runtime
	{
		namespace Renderer
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
                    vk::Fence renderFence;
                    vk::Semaphore presentSemaphore;
                    vk::Semaphore renderSemaphore;
                };

                struct Bundle
                {
                public:
                    vk::SwapchainKHR instance;
                    std::vector<Frame> frames;
                    vk::Format format;
                    vk::Extent2D extent;
                };

                void querySupport(
                    SupportDetails& outSupportDetails,
                    vk::PhysicalDevice& inPhysicalDevice,
                    vk::SurfaceKHR& inSurface
                );

                void pickSurfaceFormat(
                    vk::SurfaceFormatKHR& outSurfaceFormat,
                    std::vector<vk::SurfaceFormatKHR>& inSurfaceFormats
                );
                void pickPresentMode(
                    vk::PresentModeKHR& outPresentMode,
                    std::vector<vk::PresentModeKHR>& inPresentModes
                );
                void pickExtent(
                    vk::Extent2D& outExtent,
                    uint32_t inWidth,
                    uint32_t inHeight,
                    vk::SurfaceCapabilitiesKHR& inCapabilities
                );

				void init(
					SwapChain::Bundle& outSwapChain,
					vk::PhysicalDevice& inPhysicalDevice,
					vk::Device& inLogicalDevice,
					vk::SurfaceKHR& inSurface,
					int inWidth,
					int inHeight
				);
			}
		}
	}
}