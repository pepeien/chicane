#include "Chicane/Renderer/Backend/Vulkan/Swapchain.hpp"

#include <algorithm>
#include <array>
#include <limits>
#include <string>

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

            vk::Extent2D chooseExtent(
                const vk::SurfaceCapabilitiesKHR& inCapabilities, const vk::Extent2D& inFallback
            )
            {
                if (inCapabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max() &&
                    inCapabilities.currentExtent.height != std::numeric_limits<std::uint32_t>::max())
                {
                    return inCapabilities.currentExtent;
                }

                vk::Extent2D extent;
                extent.width = std::clamp(
                    inFallback.width,
                    inCapabilities.minImageExtent.width,
                    inCapabilities.maxImageExtent.width
                );
                extent.height = std::clamp(
                    inFallback.height,
                    inCapabilities.minImageExtent.height,
                    inCapabilities.maxImageExtent.height
                );

                return extent;
            }

            vk::CompositeAlphaFlagBitsKHR pickCompositeAlpha(vk::CompositeAlphaFlagsKHR inSupported)
            {
                const std::array<vk::CompositeAlphaFlagBitsKHR, 4> options = {
                    vk::CompositeAlphaFlagBitsKHR::eOpaque,
                    vk::CompositeAlphaFlagBitsKHR::ePreMultiplied,
                    vk::CompositeAlphaFlagBitsKHR::ePostMultiplied,
                    vk::CompositeAlphaFlagBitsKHR::eInherit
                };

                for (vk::CompositeAlphaFlagBitsKHR option : options)
                {
                    if (inSupported & option)
                    {
                        return option;
                    }
                }

                return vk::CompositeAlphaFlagBitsKHR::eOpaque;
            }

            bool init(
                VulkanSwapchainBundle&    outSwapChain,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::Device&         inLogicalDevice,
                const vk::SurfaceKHR&     inSurface,
                const vk::SwapchainKHR&   inOldSwapchain,
                const vk::Extent2D&       inFallbackExtent
            )
            {
                VulkanQueueFamilyIndices familyIndices(inPhysicalDevice, inSurface);
                const std::array<std::uint32_t, 2> queueFamilyIndices = {
                    familyIndices.graphicsFamily.value(),
                    familyIndices.presentFamily.value()
                };

                vk::SwapchainKHR     created;
                vk::SurfaceFormatKHR surfaceFormat;
                vk::Format           depthFormat = vk::Format::eUndefined;
                vk::Extent2D         extent      = {};
                vk::Result           result      = vk::Result::eErrorOutOfDateKHR;

                for (int attempt = 0; attempt < 3; ++attempt)
                {
                    VulkanSwapchainSupportDetails supportDetails;
                    querySupport(supportDetails, inPhysicalDevice, inSurface);

                    pickSurfaceFormat(surfaceFormat, supportDetails.formats);

                    vk::PresentModeKHR presentMode;
                    pickPresentMode(presentMode, supportDetails.presentModes);

                    depthFormat = VulkanImage::findSupportedFormat(
                        inPhysicalDevice,
                        {vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint, vk::Format::eD32Sfloat},
                        vk::ImageTiling::eOptimal,
                        vk::FormatFeatureFlagBits::eDepthStencilAttachment
                    );

                    extent = chooseExtent(supportDetails.capabilities, inFallbackExtent);
                    if (extent.width == 0 || extent.height == 0)
                    {
                        return false;
                    }

                    std::uint32_t imageCount = supportDetails.capabilities.minImageCount + 1;
                    if (supportDetails.capabilities.maxImageCount > 0)
                    {
                        imageCount = std::min(imageCount, supportDetails.capabilities.maxImageCount);
                    }

                    vk::ImageUsageFlags imageUsage = vk::ImageUsageFlagBits::eColorAttachment |
                                                     vk::ImageUsageFlagBits::eTransferSrc |
                                                     vk::ImageUsageFlagBits::eTransferDst;
                    imageUsage &= supportDetails.capabilities.supportedUsageFlags;
                    imageUsage |= vk::ImageUsageFlagBits::eColorAttachment;

                    vk::SwapchainCreateInfoKHR createInfo{};
                    createInfo.surface          = inSurface;
                    createInfo.minImageCount    = imageCount;
                    createInfo.imageFormat      = surfaceFormat.format;
                    createInfo.imageColorSpace  = surfaceFormat.colorSpace;
                    createInfo.imageExtent      = extent;
                    createInfo.imageArrayLayers = 1;
                    createInfo.imageUsage       = imageUsage;
                    createInfo.imageSharingMode = vk::SharingMode::eExclusive;

                    if (queueFamilyIndices[0] != queueFamilyIndices[1])
                    {
                        createInfo.imageSharingMode      = vk::SharingMode::eConcurrent;
                        createInfo.queueFamilyIndexCount = 2;
                        createInfo.pQueueFamilyIndices   = queueFamilyIndices.data();
                    }

                    createInfo.preTransform   = supportDetails.capabilities.currentTransform;
                    createInfo.compositeAlpha = pickCompositeAlpha(supportDetails.capabilities.supportedCompositeAlpha);
                    createInfo.presentMode    = presentMode;
                    createInfo.clipped        = VK_TRUE;
                    createInfo.oldSwapchain   = inOldSwapchain;

                    result = inLogicalDevice.createSwapchainKHR(&createInfo, nullptr, &created);
                    if (result == vk::Result::eSuccess)
                    {
                        break;
                    }

                    if (result != vk::Result::eErrorOutOfDateKHR)
                    {
                        throw std::runtime_error("Failed to create the swapchain: " + vk::to_string(result));
                    }
                }

                if (result != vk::Result::eSuccess)
                {
                    return false;
                }

                outSwapChain.instance    = created;
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

                return true;
            }
        }
    }
}