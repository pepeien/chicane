#pragma once

#include <cstddef>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Buffer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanLSceneLineOverlayBuffer
        {
        public:
            VulkanBuffer  buffer      = {};
            std::size_t   capacity    = 0;
            std::uint32_t vertexCount = 0;
        };
    }
}
