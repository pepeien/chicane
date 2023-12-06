#include "SwapChain.hpp"

namespace Engine
{
	namespace Core
	{
		namespace Render
		{
			namespace SwapChain
			{
				void init(
					Bundle& allocator,
					vk::PhysicalDevice& inPhysicalDevice,
					vk::Device& inLogicalDevice,
					vk::SurfaceKHR& inSurface,
					int inWidth,
					int inHeight
				)
				{
					SupportDetails supportDetails;
					querySupport(supportDetails, inPhysicalDevice, inSurface);

					vk::SurfaceFormatKHR surfaceFormat;
					pickSurfaceFormat(surfaceFormat, supportDetails.formats);

					vk::PresentModeKHR presentMode;
					pickPresentMode(presentMode, supportDetails.presentModes);

					vk::Extent2D extent;
					pickExtent(extent, inWidth, inHeight, supportDetails.capabilities);

					uint32_t imageCount = std::min(
						supportDetails.capabilities.maxImageCount,
						supportDetails.capabilities.minImageCount + 1
					);
					// We don't need more than double buffering
					imageCount = std::min(
						imageCount,
						MAX_BUFFER_MULTIPLIER
					);
	
					vk::SwapchainCreateInfoKHR createInfo = vk::SwapchainCreateInfoKHR(
						vk::SwapchainCreateFlagsKHR(),
						inSurface,
						imageCount,
						surfaceFormat.format,
						surfaceFormat.colorSpace,
						extent,
						1,
						vk::ImageUsageFlagBits::eColorAttachment
					);

					Queue::FamilyIndices familyIndices;
					Queue::findFamilyInidices(familyIndices, inPhysicalDevice, inSurface);

					createInfo.imageSharingMode = vk::SharingMode::eExclusive;

					if (familyIndices.graphicsFamily.value() != familyIndices.presentFamily.value())
					{
						uint32_t queueFamilyIndices[] = { familyIndices.graphicsFamily.value(), familyIndices.presentFamily.value() };

						createInfo.imageSharingMode      = vk::SharingMode::eConcurrent;
						createInfo.queueFamilyIndexCount = 2;
						createInfo.pQueueFamilyIndices   = queueFamilyIndices;
					}

					createInfo.preTransform   = supportDetails.capabilities.currentTransform;
					createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
					createInfo.presentMode    = presentMode;
					createInfo.clipped        = VK_TRUE;
					createInfo.oldSwapchain   = vk::SwapchainKHR(nullptr);

					allocator.swapchain = inLogicalDevice.createSwapchainKHR(createInfo);
					allocator.format    = surfaceFormat.format;
					allocator.extent    = extent;

					std::vector<vk::Image> images = inLogicalDevice.getSwapchainImagesKHR(allocator.swapchain);
					allocator.frames.resize(images.size());

					for (int i = 0; i < images.size(); i++)
					{
						vk::ImageViewCreateInfo createInfo = {};
						createInfo.image    = images[i];
						createInfo.viewType = vk::ImageViewType::e2D;
						createInfo.format   = surfaceFormat.format;

						createInfo.components.r = vk::ComponentSwizzle::eIdentity;
						createInfo.components.g = vk::ComponentSwizzle::eIdentity;
						createInfo.components.r = vk::ComponentSwizzle::eIdentity;
						createInfo.components.a = vk::ComponentSwizzle::eIdentity;

						createInfo.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
						createInfo.subresourceRange.baseMipLevel   = 0;
						createInfo.subresourceRange.levelCount     = 1;
						createInfo.subresourceRange.baseArrayLayer = 0;
						createInfo.subresourceRange.layerCount     = 1;

						allocator.frames[i].image     = images[i];
						allocator.frames[i].imageView = inLogicalDevice.createImageView(createInfo);
					}
				}
			}
		}
	}
}