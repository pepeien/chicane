#include "Chicane/Renderer/SwapChain.hpp"

constexpr uint32_t MAX_BUFFER_MULTIPLIER = 3;

namespace Chicane
{
    namespace SwapChain
    {
        void querySupport(
            SupportDetails& outSupportDetails,
            const vk::PhysicalDevice& inPhysicalDevice,
            const vk::SurfaceKHR& inSurface
        )
        {
            outSupportDetails.capabilities = inPhysicalDevice.getSurfaceCapabilitiesKHR(
                inSurface
            );
            outSupportDetails.formats = inPhysicalDevice.getSurfaceFormatsKHR(
                inSurface
            );
            outSupportDetails.presentModes = inPhysicalDevice.getSurfacePresentModesKHR(
                inSurface
            );
        }

        void pickSurfaceFormat(
            vk::SurfaceFormatKHR& outSurfaceFormat,
            const std::vector<vk::SurfaceFormatKHR>& inSurfaceFormats
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
            const std::vector<vk::PresentModeKHR>& inPresentModes
        )
        {
            bool doesSupportMailBox = std::find(
                inPresentModes.begin(),
                inPresentModes.end(),
                vk::PresentModeKHR::eMailbox
            ) != inPresentModes.end();
    
            if (doesSupportMailBox)
            {
                outPresentMode = vk::PresentModeKHR::eMailbox;
    
                return;
            }

            // Due to AMD's lack of support to mailbox mode I will use Immediate as a alternative
            bool doesSupportImmediate = std::find(
                inPresentModes.begin(),
                inPresentModes.end(),
                vk::PresentModeKHR::eImmediate
            ) != inPresentModes.end();

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
            const vk::SurfaceCapabilitiesKHR& inCapabilities
        )
        {
            if (inCapabilities.currentExtent.width < UINT32_MAX)
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
            const vk::PhysicalDevice& inPhysicalDevice,
            const vk::Device& inLogicalDevice,
            const vk::SurfaceKHR& inSurface,
            const Vec<int>::Two& inResolution
        )
        {
            SupportDetails supportDetails;
            querySupport(
                supportDetails,
                inPhysicalDevice,
                inSurface
            );

            vk::SurfaceFormatKHR surfaceFormat;
            pickSurfaceFormat(
                surfaceFormat,
                supportDetails.formats
            );

            vk::PresentModeKHR presentMode;
            pickPresentMode(
                presentMode,
                supportDetails.presentModes
            );

            vk::Extent2D extent;
            pickExtent(
                extent,
                inResolution.x,
                inResolution.y,
                supportDetails.capabilities
            );

            uint32_t imageCount = supportDetails.capabilities.minImageCount + 1;

            if (supportDetails.capabilities.maxImageCount > 0)
            {
                if (imageCount > supportDetails.capabilities.maxImageCount)
                {
                    imageCount = supportDetails.capabilities.maxImageCount;
                }
                else
                {
                    imageCount = std::min(
                        supportDetails.capabilities.maxImageCount,
                        MAX_BUFFER_MULTIPLIER
                    );
                }
            }
            else
            {
                imageCount = std::max(
                    supportDetails.capabilities.minImageCount,
                    MAX_BUFFER_MULTIPLIER
                );
            }

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
            Queue::findFamilyInidices(
                familyIndices,
                inPhysicalDevice,
                inSurface
            );

            createInfo.imageSharingMode = vk::SharingMode::eExclusive;
    
            if (familyIndices.graphicsFamily.value() != familyIndices.presentFamily.value())
            {
                uint32_t queueFamilyIndices[] = {
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
    
            outSwapChain.instance         = inLogicalDevice.createSwapchainKHR(createInfo);
            outSwapChain.format           = surfaceFormat.format;
            outSwapChain.extent           = extent;
            outSwapChain.midPoints.width  = extent.width / 2;
            outSwapChain.midPoints.height = extent.height / 2;
    
            std::vector<vk::Image> images = inLogicalDevice.getSwapchainImagesKHR(
                outSwapChain.instance
            );
            outSwapChain.images.resize(images.size());
    
            for (int i = 0; i < images.size(); i++)
            {
                Frame::Instance& frame = outSwapChain.images[i];
                frame.image            = images[i];
                frame.width            = extent.width;
                frame.height           = extent.height;
                frame.physicalDevice   = inPhysicalDevice;
                frame.logicalDevice    = inLogicalDevice;

                Image::initView(
                    frame.imageView,
                    inLogicalDevice,
                    frame.image,
                    surfaceFormat.format,
                    vk::ImageAspectFlagBits::eColor,
                    vk::ImageViewType::e2D,
                    1
                );

                frame.setupDepthBuffering();

                frame.setupCameraUBO();
            }
        }
    }
}