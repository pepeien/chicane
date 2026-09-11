#pragma once

#include <cstddef>
#include <cstdint>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Buffer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanLSceneLineImmediateBuffer
        {
        public:
            VulkanBuffer vertexBuffer   = {};
            void*        vertexMapped   = nullptr;
            std::size_t  vertexCapacity = 0;

            VulkanBuffer indexBuffer   = {};
            void*        indexMapped   = nullptr;
            std::size_t  indexCapacity = 0;
        };
    }
}
