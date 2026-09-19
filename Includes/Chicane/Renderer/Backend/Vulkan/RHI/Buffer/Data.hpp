#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Buffer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanRHIBufferData
        {
        public:
            VulkanBuffer   buffer = {};
            vk::DeviceSize size   = 0;
            bool           bHost  = false;
            bool           bOwned = true;
            void*          mapped = nullptr;
        };
    }
}
