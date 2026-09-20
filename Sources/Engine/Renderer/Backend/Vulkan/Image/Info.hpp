#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Core/Image/Info.hpp"

#include "Chicane/Renderer.hpp"

struct VmaAllocator_T;
struct VmaAllocation_T;

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanImageInfo
        {
        public:
            vk::Image        instance   = nullptr;
            vk::ImageView    view       = nullptr;
            vk::DeviceMemory memory     = nullptr;
            vk::Sampler      sampler    = nullptr;
            vk::Format       format     = vk::Format::eUndefined;
            vk::Extent2D     extent     = {};
            VmaAllocator_T*  allocator  = nullptr;
            VmaAllocation_T* allocation = nullptr;
        };
    }
}