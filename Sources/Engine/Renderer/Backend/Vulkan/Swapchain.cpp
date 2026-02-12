#include "Chicane/Renderer/Backend/Vulkan/Swapchain.hpp"

#include <algorithm>

namespace Chicane
{
    namespace Renderer
    {
        namespace VulkanSwapchain
        {
            void querySupport(
                VulkanSwapchainSupportDetails& outSupportDetails,
                const vk::PhysicalDevice&      inPhysicalDevice,
                const vk::SurfaceKHR&          inSurface
            )
            {
                outSupportDetails.capabilities = inPhysicalDevice.getSurfaceCapabilitiesKHR(inSurface);
                outSupportDetails.formats      = inPhysicalDevice.getSurfaceFormatsKHR(inSurface);
                outSupportDetails.presentModes = inPhysicalDevice.getSurfacePresentModesKHR(inSurface);
            }

            void pickSurfaceFormat(
                vk::SurfaceFormatKHR& outSurfaceFormat, const std::vector<vk::SurfaceFormatKHR>& inSurfaceFormats
            )
            {
                if (inSurfaceFormats.empty())
                {
                    throw std::runtime_error("There is no surface formats available");
                }

                for (vk::SurfaceFormatKHR surfaceFormat : inSurfaceFormats)
                {
                    if (surfaceFormat.format == vk::Format::eB8G8R8A8Unorm &&
                        surfaceFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
                    {
                        outSurfaceFormat = surfaceFormat;

                        return;
                    }
                }

                outSurfaceFormat = inSurfaceFormats[0];
            }

            void pickPresentMode(
                vk::PresentModeKHR& outPresentMode, const std::vector<vk::PresentModeKHR>& inPresentModes
            )
            {
                bool bDoesSupportMailBox =
                    std::find(inPresentModes.begin(), inPresentModes.end(), vk::PresentModeKHR::eMailbox) !=
                    inPresentModes.end();

                if (bDoesSupportMailBox)
                {
                    outPresentMode = vk::PresentModeKHR::eMailbox;

                    return;
                }

                // Due to AMD's lack of support to mailbox mode I will use
                // Immediate as a alternative
                bool bDoesSupportImmediate =
                    std::find(inPresentModes.begin(), inPresentModes.end(), vk::PresentModeKHR::eImmediate) !=
                    inPresentModes.end();

                if (bDoesSupportImmediate)
                {
                    outPresentMode = vk::PresentModeKHR::eImmediate;

                    return;
                }

                outPresentMode = vk::PresentModeKHR::eFifo;
            }

            void init(
                VulkanSwapchainBundle&    outSwapChain,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::Device&         inLogicalDevice,
                const vk::SurfaceKHR&     inSurface
            )
            {
                VulkanSwapchainSupportDetails supportDetails;
                querySupport(supportDetails, inPhysicalDevice, inSurface);

                vk::SurfaceFormatKHR surfaceFormat;
                pickSurfaceFormat(surfaceFormat, supportDetails.formats);

                vk::Format depthFormat = VulkanImage::findSupportedFormat(
                    inPhysicalDevice,
                    {vk::Format::eD32Sfloat, vk::Format::eD24UnormS8Uint},
                    vk::ImageTiling::eOptimal,
                    vk::FormatFeatureFlagBits::eDepthStencilAttachment
                );

                vk::PresentModeKHR presentMode;
                pickPresentMode(presentMode, supportDetails.presentModes);

                vk::Extent2D extent = supportDetails.capabilities.currentExtent;

                std::uint32_t imageCount = supportDetails.capabilities.minImageCount;

                vk::SwapchainCreateInfoKHR createInfo = {};
                createInfo.surface                    = inSurface;
                createInfo.minImageCount              = imageCount;
                createInfo.imageFormat                = surfaceFormat.format;
                createInfo.imageColorSpace            = surfaceFormat.colorSpace;
                createInfo.imageExtent                = extent;
                createInfo.imageArrayLayers           = 1;
                createInfo.imageUsage                 = vk::ImageUsageFlagBits::eColorAttachment;

                VulkanQueueFamilyIndices familyIndices(inPhysicalDevice, inSurface);

                createInfo.imageSharingMode = vk::SharingMode::eExclusive;

                if (familyIndices.graphicsFamily.value() != familyIndices.presentFamily.value())
                {
                    std::uint32_t queueFamilyIndices[] = {
                        familyIndices.graphicsFamily.value(),
                        familyIndices.presentFamily.value()
                    };

                    createInfo.imageSharingMode      = vk::SharingMode::eConcurrent;
                    createInfo.queueFamilyIndexCount = 2;
                    createInfo.pQueueFamilyIndices   = queueFamilyIndices;
                }

                createInfo.preTransform   = supportDetails.capabilities.currentTransform;
                createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
                createInfo.presentMode    = presentMode;
                createInfo.clipped        = VK_TRUE;
                createInfo.oldSwapchain   = vk::SwapchainKHR(nullptr);

                outSwapChain.instance    = inLogicalDevice.createSwapchainKHR(createInfo);
                outSwapChain.colorFormat = surfaceFormat.format;
                outSwapChain.depthFormat = depthFormat;
                outSwapChain.extent      = extent;

                std::vector<vk::Image> images = inLogicalDevice.getSwapchainImagesKHR(outSwapChain.instance);
                outSwapChain.images.resize(images.size());

                for (int i = 0; i < images.size(); i++)
                {
                    VulkanSwapchainImage& image = outSwapChain.images[i];
                    image.physicalDevice        = inPhysicalDevice;
                    image.logicalDevice         = inLogicalDevice;
                    image.colorImage.instance   = images[i];
                }

                outSwapChain.currentImageIndex = 0U;
            }
        }
    }
}