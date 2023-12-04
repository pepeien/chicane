#include "SwapChain.h"

namespace Engine
{
	namespace Core
	{
		namespace Vulkan
		{
			namespace SwapChain
        	{
				void querySupport(SupportDetails& allocator, vk::PhysicalDevice& inPhyisicalDevice, vk::SurfaceKHR& inSurface)
				{
					allocator.capabilities = inPhyisicalDevice.getSurfaceCapabilitiesKHR(inSurface);
					allocator.formats      = inPhyisicalDevice.getSurfaceFormatsKHR(inSurface);
					allocator.presentModes = inPhyisicalDevice.getSurfacePresentModesKHR(inSurface);
				}

				void pickSurfaceFormat(vk::SurfaceFormatKHR& allocator, std::vector<vk::SurfaceFormatKHR>& inSurfaceFormats)
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
							allocator = surfaceFormat;

							return;
						}
					}

					allocator = inSurfaceFormats[0];
				}

				void pickPresentMode(vk::PresentModeKHR& allocator, std::vector<vk::PresentModeKHR>& inPresentModes)
				{
					for (vk::PresentModeKHR presentMode : inPresentModes)
					{
						if (presentMode == vk::PresentModeKHR::eMailbox)
						{
							allocator = presentMode;

							return;
						}
					}

					allocator = vk::PresentModeKHR::eFifo;
				}

				void pickExtent(vk::Extent2D& allocator, uint32_t inWidth, uint32_t inHeight, vk::SurfaceCapabilitiesKHR& inCapabilities)
				{
					if (inCapabilities.currentExtent.width != UINT32_MAX)
					{
						allocator = inCapabilities.currentExtent;

						return;
					}

					allocator.setWidth(
						std::min(
							inCapabilities.maxImageExtent.width,
							std::max(inCapabilities.minImageExtent.width, inWidth)
						)
					);

					allocator.setHeight(
						std::min(
							inCapabilities.maxImageExtent.height,
							std::max(inCapabilities.minImageExtent.height, inHeight)
						)
					);
				}
			}
		}
	}
}