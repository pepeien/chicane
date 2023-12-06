#include "SwapChain.hpp"

namespace Engine
{
	namespace Core
	{
		namespace Render
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
					bool doesSupportMailBox = std::find(inPresentModes.begin(), inPresentModes.end(), vk::PresentModeKHR::eMailbox) != inPresentModes.end();

					if (doesSupportMailBox)
					{
						Log::info("GPU is using Mailbox present mode");

						allocator = vk::PresentModeKHR::eMailbox;

						return;
					}

					// Due to AMD's lack of support to mailbox mode I will use Immediate as a alternative
					bool doesSupportImmediate = std::find(inPresentModes.begin(), inPresentModes.end(), vk::PresentModeKHR::eImmediate) != inPresentModes.end();

					if (doesSupportImmediate)
					{
						Log::info("GPU is using Immediate present mode");

						allocator = vk::PresentModeKHR::eImmediate;

						return;
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