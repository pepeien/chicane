#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Renderer/Frame.hpp"
#include "Chicane/Renderer/Queue.hpp"
#include "Chicane/Renderer/Image.hpp"

namespace Chicane
{
    namespace SwapChain
    {
        struct SupportDetails
        {
            vk::SurfaceCapabilitiesKHR        capabilities;
            std::vector<vk::SurfaceFormatKHR> formats;
            std::vector<vk::PresentModeKHR>   presentModes;
        };

        struct Bundle
        {
            vk::SwapchainKHR             instance;
            std::vector<Frame::Instance> frames;
            vk::Format                   format;
            vk::Format                   depthFormat;
            vk::Extent2D                 extent;
            vk::Extent2D                 midPoints;
        };

        void querySupport(
            SupportDetails& outSupportDetails,
            const vk::PhysicalDevice& inPhysicalDevice,
            const vk::SurfaceKHR& inSurface
        );
    
        void pickSurfaceFormat(
            vk::SurfaceFormatKHR& outSurfaceFormat,
            const std::vector<vk::SurfaceFormatKHR>& inSurfaceFormats
        );
        void pickPresentMode(
            vk::PresentModeKHR& outPresentMode,
            const std::vector<vk::PresentModeKHR>& inPresentModes
        );
        void pickExtent(
            vk::Extent2D& outExtent,
            uint32_t inWidth,
            uint32_t inHeight,
            const vk::SurfaceCapabilitiesKHR& inCapabilities
        );

        void init(
            Bundle& outSwapChain,
            const vk::PhysicalDevice& inPhysicalDevice,
            const vk::Device& inLogicalDevice,
            const vk::SurfaceKHR& inSurface,
            const Vec<2, int>& inResolution
        );
    }
}