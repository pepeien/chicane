#pragma once

#include "Runtime/Runtime.hpp"
#include "Runtime/Renderer/Frame.hpp"
#include "Runtime/Renderer/Queue.hpp"
#include "Runtime/Renderer/Image.hpp"

namespace Chicane
{
    namespace SwapChain
    {
        struct SupportDetails
        {
            vk::SurfaceCapabilitiesKHR capabilities;
            std::vector<vk::SurfaceFormatKHR> formats;
            std::vector<vk::PresentModeKHR> presentModes;
        };

        struct Bundle
        {
            vk::SwapchainKHR instance;
            std::vector<Frame::Instance> images;
            vk::Format format;
            vk::Extent2D extent;
            vk::Extent2D midPoints;
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
            const Vec2& inResolution
        );
    }
}