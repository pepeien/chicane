#pragma once

#include <cstdint>
#include <optional>

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanQueueFamilyIndices
        {
        public:
            VulkanQueueFamilyIndices(
                const vk::PhysicalDevice& inPhysicalDevice, const vk::SurfaceKHR& inSurface
            );

        public:
            bool isComplete();

        public:
            std::optional<std::uint32_t> graphicsFamily;
            std::optional<std::uint32_t> presentFamily;
        };
    }
}