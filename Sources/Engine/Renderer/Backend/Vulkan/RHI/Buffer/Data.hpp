#pragma once

#include "Chicane/Renderer.hpp"
#include "Backend/Vulkan/Buffer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanRHIBufferData
        {
        public:
            VulkanBuffer         buffer = {};
            vk::DeviceSize       size   = 0;
            vk::BufferUsageFlags usage  = {};
            bool                 bHost  = false;
            bool                 bOwned = true;
            void*                mapped = nullptr;
        };
    }
}
