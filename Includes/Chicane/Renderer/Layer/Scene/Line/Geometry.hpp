#pragma once

#include <cstddef>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/RHI/Buffer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER LSceneLineGeometry
        {
        public:
            RHI::Buffer vertex      = {};
            RHI::Buffer index       = {};
            std::size_t vertexBytes = 0;
            std::size_t indexBytes  = 0;
        };
    }
}
