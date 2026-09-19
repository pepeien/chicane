#pragma once

#include <cstddef>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/RHI/Buffer/Usage.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            struct CHICANE_RENDERER BufferCreateInfo
            {
            public:
                bool        bHasHostAccess = false;

                std::size_t size  = 0;
                BufferUsage usage = BufferUsage::Vertex;
            };
        }
    }
}
