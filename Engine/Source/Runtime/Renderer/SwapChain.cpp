#include "SwapChain.hpp"

namespace Engine
{
	namespace Runtime
	{
		namespace Renderer
		{
			namespace SwapChain
			{
				void querySupport(
					SupportDetails& outSupportDetails,
					vk::PhysicalDevice& inPhysicalDevice,
					vk::SurfaceKHR& inSurface
				)
				{
					outSupportDetails.capabilities = inPhysicalDevice.getSurfaceCapabilitiesKHR(inSurface);
					outSupportDetails.formats      = inPhysicalDevice.getSurfaceFormatsKHR(inSurface);
					outSupportDetails.presentModes = inPhysicalDevice.getSurfacePresentModesKHR(inSurface);
				}

				void pickSurfaceFormat(
					vk::SurfaceFormatKHR& outSurfaceFormat,
					std::vector<vk::SurfaceFormatKHR>& inSurfaceFormats
				)
				{
					if (inSurfaceFormats.empty())
					{
						throw std::runtime_error("There is no surface formats available");
					}

					for (vk::SurfaceFormatKHR surfaceFormat : inSurfaceFormats)
					{
						if (
							surfaceFormat.format == vk::Format::eB8G8R8A8Unorm &&
							surfaceFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear
						)
						{
							outSurfaceFormat = surfaceFormat;

							return;
						}
					}

					outSurfaceFormat = inSurfaceFormats[0];
				}

				void pickPresentMode(
					vk::PresentModeKHR& outPresentMode,
					std::vector<vk::PresentModeKHR>& inPresentModes
				)
				{
					bool doesSupportMailBox = std::find(inPresentModes.begin(), inPresentModes.end(), vk::PresentModeKHR::eMailbox) != inPresentModes.end();

					if (doesSupportMailBox)
					{
						outPresentMode = vk::PresentModeKHR::eMailbox;

						return;
					}

					// Due to AMD's lack of support to mailbox mode I will use Immediate as a alternative
					bool doesSupportImmediate = std::find(inPresentModes.begin(), inPresentModes.end(), vk::PresentModeKHR::eImmediate) != inPresentModes.end();

					if (doesSupportImmediate)
					{
						outPresentMode = vk::PresentModeKHR::eImmediate;

						return;
					}

					outPresentMode = vk::PresentModeKHR::eFifo;
				}

				void pickExtent(
					vk::Extent2D& outExtent,
					uint32_t inWidth,
					uint32_t inHeight,
					vk::SurfaceCapabilitiesKHR& inCapabilities
				)
				{
					if (inCapabilities.currentExtent.width != UINT32_MAX)
					{
						outExtent = inCapabilities.currentExtent;

						return;
					}

					outExtent.setWidth(
						std::min(
							inCapabilities.maxImageExtent.width,
							std::max(inCapabilities.minImageExtent.width, inWidth)
						)
					);

					outExtent.setHeight(
						std::min(
							inCapabilities.maxImageExtent.height,
							std::max(inCapabilities.minImageExtent.height, inHeight)
						)
					);
				}

				void init(
					Bundle& outSwapChain,
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

					outSwapChain.instance = inLogicalDevice.createSwapchainKHR(createInfo);
					outSwapChain.format    = surfaceFormat.format;
					outSwapChain.extent    = extent;

					std::vector<vk::Image> images = inLogicalDevice.getSwapchainImagesKHR(outSwapChain.instance);
					outSwapChain.frames.resize(images.size());

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

						outSwapChain.frames[i].image     = images[i];
						outSwapChain.frames[i].imageView = inLogicalDevice.createImageView(createInfo);
					}
				}
			}
		}
	}
}