#pragma once

#include <cstddef>
#include <cstdint>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/RHI/Buffer/Usage.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER OpenGLRHIBufferData
        {
        public:
            std::uint32_t    id     = 0;
            std::size_t      size   = 0;
            RHI::BufferUsage usage  = RHI::BufferUsage::Vertex;
            bool             bOwned = true;
        };
    }
}
