#pragma once

#include <cstdint>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/RHI/Vertex/Format.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            struct CHICANE_RENDERER VertexAttribute
            {
            public:
                std::uint32_t location = 0;
                VertexFormat  format   = VertexFormat::Float3;
                std::uint32_t offset   = 0;
            };
        }
    }
}
